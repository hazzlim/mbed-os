/*
 * Copyright (c) 2021, Arm Limited and affiliates
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RTPMIDI.h"

#include "gmock/gmock.h"

using ::testing::Eq;

MATCHER_P(Equals, expected, "")
{
    return arg.command_header.signature == expected.command_header.signature &&
           arg.command_header.command == expected.command_header.command     &&
           arg.protocol_version == expected.protocol_version                 &&
           arg.initiator_token == expected.initiator_token                   &&
           arg.sender_ssrc == expected.sender_ssrc                           &&
           (0 == strcmp(arg.name, expected.name));
}

#define TEST_INITIATOR_TOKEN    0x327b23c6
#define TEST_INITIATOR_SSRC     0xa556f4da
#define TEST_INITIATOR_NAME     "HOST"
#define TEST_SSRC               0xdbffa3a1
#define TEST_NAME               "NAME"

const exchange_packet_t invitation_packet = {
    SIGNATURE,
    INV,
    PROTOCOL_VERSION,
    TEST_INITIATOR_TOKEN,
    TEST_INITIATOR_SSRC,
    TEST_INITIATOR_NAME
};
const exchange_packet_t expected_response_packet = {
    SIGNATURE,
    ACCEPT_INV,
    PROTOCOL_VERSION,
    TEST_INITIATOR_TOKEN,
    TEST_SSRC,
    TEST_NAME
};

class NetworkInterface_Stub : public NetworkInterface {
public:
    bool is_connected {false};

    nsapi_error_t connect() override
    {
        is_connected = true;
        return NSAPI_ERROR_OK;
    }

    nsapi_error_t disconnect() override
    {
        return NSAPI_ERROR_OK;
    }

    NetworkStack* get_stack() override
    {
        return nullptr;
    }
};

class UDPSocket_Stub : public UDPSocket {
public:
    exchange_packet_t invitation_to_return;
    exchange_packet_t expected_response;
    exchange_packet_t response_one;
    exchange_packet_t response_two;

    size_t send_count {};

    void set_return(const exchange_packet_t &ret)
    {
        invitation_to_return = ret;
        to_network_order(invitation_to_return);
    }

    void expect_response(const exchange_packet_t &exp)
    {
        expected_response = exp;
        to_network_order(expected_response);
    }

    nsapi_size_or_error_t recvfrom(SocketAddress *address,
                                           void *data, nsapi_size_t size)
    {
        memcpy(data, &invitation_to_return, size);
        return NSAPI_ERROR_OK;
    }

    nsapi_size_or_error_t sendto(const SocketAddress &address,
                                         const void *data, nsapi_size_t size)
    {
        ++send_count;
        exchange_packet_t *response = (send_count == 1) ? &response_one : &response_two;

        memcpy(response, data, size);
        return NSAPI_ERROR_OK;
    }
};

class TestRTPMIDI : public testing::Test {
    public:
        RTPMIDI rtpmidi{TEST_SSRC, TEST_NAME};

        exchange_packet_t response;
        exchange_packet_t invitation = invitation_packet;
        exchange_packet_t expected_response = expected_response_packet;
};



TEST_F(TestRTPMIDI, GeneratesAcceptInvitationPacket)
{
    auto error = rtpmidi.accept_response(invitation, response);

    ASSERT_THAT(error, Eq(RTPMIDI_ERROR_OK));
    ASSERT_THAT(response, Equals(expected_response));
}

TEST_F(TestRTPMIDI, ErrorOnInvalidInvitationPacket)
{
    // Invalid Command
    invitation.command_header.command = 0;
    auto error = rtpmidi.accept_response(invitation, response);

    ASSERT_THAT(error, Eq(RTPMIDI_ERROR_COMMAND));
}

TEST_F(TestRTPMIDI, GeneratesResponseToSynchronizationPacket)
{
    synchronization_packet_t initiator_packet;
    synchronization_packet_t response_packet;

    // Set up valid sync packet
    constexpr uint64_t initiator_timestamp = 1234;
    constexpr uint64_t response_timestamp = 4321;
    initiator_packet.command_header.command = CK;
    initiator_packet.count = SYNC0;
    initiator_packet.timestamp[SYNC0] = initiator_timestamp;

    auto error = rtpmidi.synchronization_response(initiator_packet, response_packet, response_timestamp);

    ASSERT_EQ(error, RTPMIDI_ERROR_OK);
    ASSERT_EQ(response_packet.command_header.signature, SIGNATURE);
    ASSERT_EQ(response_packet.command_header.command, CK);
    ASSERT_EQ(response_packet.sender_ssrc, rtpmidi.ssrc());
    ASSERT_EQ(response_packet.count, SYNC1);
    ASSERT_EQ(response_packet.timestamp[SYNC0], initiator_packet.timestamp[SYNC0]);
    ASSERT_EQ(response_packet.timestamp[SYNC1], response_timestamp);
}

TEST_F(TestRTPMIDI, ErrorOnInvalidSynchronizationPacket)
{
    synchronization_packet_t initiator_packet;
    synchronization_packet_t response_packet;

    // Unexpected count value
    initiator_packet.count = SYNC1;

    constexpr uint32_t irrelevant_timestamp = 0;
    auto error = rtpmidi.synchronization_response(initiator_packet, response_packet, irrelevant_timestamp);

    ASSERT_EQ(error, RTPMIDI_ERROR_SYNC);
}

TEST_F(TestRTPMIDI, GeneratesMIDIPacketHeader)
{
    midi_packet_header_t header;
    uint16_t sequence_number = 1;
    uint32_t timestamp = 1234;
    rtpmidi.generate_midi_header(header, sequence_number, timestamp);

    ASSERT_EQ(header.vpxcc, VPXCC);
    ASSERT_EQ(header.mpayload, MPAYLOAD);
    ASSERT_EQ(header.sequence_number, sequence_number);
    ASSERT_EQ(header.timestamp, timestamp);
    ASSERT_EQ(header.sender_ssrc, rtpmidi.ssrc());
}

TEST_F(TestRTPMIDI, CorrectlyEstablishesSession)
{
    NetworkInterface_Stub net_stub;
    UDPSocket_Stub socket_stub;
    socket_stub.set_return(invitation_packet);
    socket_stub.expect_response(expected_response_packet);

    rtpmidi.bind_net(&net_stub);
    rtpmidi.bind_socket(&socket_stub);
    rtpmidi.participate();

    ASSERT_THAT(net_stub.is_connected, Eq(true));
    //ASSERT_THAT(socket_stub.is_open, Eq(true));

    //ASSERT_THAT(socket_stub.send_count, Eq(2));
    ASSERT_THAT(socket_stub.response_one, Equals(socket_stub.expected_response));
    //ASSERT_THAT(socket_stub.response_two, Equals(socket_stub.expected_response));
}
