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

#define TEST_INITIATOR_TOKEN    0x327b23c6
#define TEST_INITIATOR_SSRC     0xa556f4da
#define TEST_INITIATOR_NAME     "HOST"
#define TEST_SSRC               0xdbffa3a1
#define TEST_NAME               "NAME"

class UDPSocket_Stub : public UDPSocket {
public:
    nsapi_size_or_error_t recvfrom(SocketAddress *address,
                                           void *data, nsapi_size_t size)
    {
        to_network_order(invitation);
        memcpy(data, &invitation, size);
        return NSAPI_ERROR_OK;
    }

    nsapi_size_or_error_t sendto(const SocketAddress &address,
                                         const void *data, nsapi_size_t size)
    {
        to_network_order(expected_response);

        memcpy(&actual_response, data, size);
        return NSAPI_ERROR_OK;
    }

    exchange_packet_t actual_response;
    exchange_packet_t invitation = {
        SIGNATURE,
        INV,
        PROTOCOL_VERSION,
        TEST_INITIATOR_TOKEN,
        TEST_INITIATOR_SSRC,
        TEST_INITIATOR_NAME
    };

    exchange_packet_t expected_response = {
        SIGNATURE,
        ACCEPT_INV,
        PROTOCOL_VERSION,
        TEST_INITIATOR_TOKEN,
        TEST_SSRC,
        TEST_NAME
    };

};

MATCHER_P(Equals, expected, "")
{
    return arg.command_header.signature == expected.command_header.signature &&
           arg.command_header.command == expected.command_header.command     &&
           arg.protocol_version == expected.protocol_version                 &&
           arg.initiator_token == expected.initiator_token                   &&
           arg.sender_ssrc == expected.sender_ssrc                           &&
           (0 == strcmp(arg.name, expected.name));
}

class TestRTPMIDI : public testing::Test {
    public:
        UDPSocket_Stub socket_stub;
        RTPMIDI rtpmidi{&socket_stub, TEST_SSRC, TEST_NAME};

        exchange_packet_t invitation_packet;
        exchange_packet_t response_packet;
};

TEST_F(TestRTPMIDI, GeneratesAcceptInvitationPacket)
{
    // Invitation Command
    invitation_packet.command_header.command = INV;
    auto error = rtpmidi.accept_response(invitation_packet, response_packet);

    ASSERT_EQ(error, RTPMIDI_ERROR_OK);
    ASSERT_EQ(response_packet.command_header.signature, SIGNATURE);
    ASSERT_EQ(response_packet.command_header.command, ACCEPT_INV);
    ASSERT_EQ(response_packet.protocol_version, PROTOCOL_VERSION);
    ASSERT_EQ(response_packet.initiator_token, invitation_packet.initiator_token);
    ASSERT_EQ(response_packet.sender_ssrc, rtpmidi.ssrc());
    ASSERT_EQ(response_packet.name, rtpmidi.name());

}

TEST_F(TestRTPMIDI, ErrorOnInvalidInvitationPacket)
{
    // Invalid Command
    invitation_packet.command_header.command = 0;
    auto error = rtpmidi.accept_response(invitation_packet, response_packet);

    ASSERT_EQ(error, RTPMIDI_ERROR_COMMAND);
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

TEST_F(TestRTPMIDI, SendsCorrectResponseToInvitation)
{
    rtpmidi.participate();

    ASSERT_THAT(socket_stub.actual_response, Equals(socket_stub.expected_response));
}
