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

using testing::Eq;
using testing::Return;
using testing::_;
using testing::NotNull;
using testing::DoAll;
using testing::SetArgPointee;
using testing::SaveArg;
using testing::Sequence;

#define TEST_INITIATOR_TOKEN    0x327b23c6
#define TEST_INITIATOR_SSRC     0xa556f4da
#define TEST_INITIATOR_NAME     "HOST"
#define TEST_SSRC               0xdbffa3a1
#define TEST_NAME               "NAME"
#define TEST_TIMESTAMP_0        1
#define TEST_TIMESTAMP_2        2
/* Value returned by now() stub, converted to units of 100us */
#define KERNEL_CLOCK_NOW        200

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

const synchronization_packet_t sync_packet_0 = {
    SIGNATURE,
    CK,
    TEST_INITIATOR_SSRC,
    SYNC0,
    0, 0, 0,
    TEST_TIMESTAMP_0
};

const synchronization_packet_t sync_packet_1 = {
    SIGNATURE,
    CK,
    TEST_SSRC,
    SYNC1,
    0, 0, 0,
    TEST_TIMESTAMP_0,
    KERNEL_CLOCK_NOW
};

const synchronization_packet_t sync_packet_2 = {
    SIGNATURE,
    CK,
    TEST_INITIATOR_SSRC,
    SYNC2,
    0, 0, 0,
    TEST_TIMESTAMP_0,
    KERNEL_CLOCK_NOW,
    TEST_TIMESTAMP_2
};

const midi_packet_header_t midi_packet_header = {
    VPXCC,
    MPAYLOAD,
    0,
    KERNEL_CLOCK_NOW,
    TEST_SSRC
};
/* Size of header, message header and one message */
constexpr size_t size_of_midi_packet = 4 + 1 + sizeof(midi_packet_header_t);

/* Byte Swapping Functions */
void swap_bytes(exchange_packet_t &packet)
{
    packet.command_header.signature = htons(packet.command_header.signature);
    packet.command_header.command = htons(packet.command_header.command);
    packet.protocol_version = htonl(packet.protocol_version);
    packet.initiator_token = htonl(packet.initiator_token);
    packet.sender_ssrc = htonl(packet.sender_ssrc);
}
void swap_bytes(synchronization_packet_t &packet)
{
    packet.command_header.signature = htons(packet.command_header.signature);
    packet.command_header.command = htons(packet.command_header.command);
    packet.sender_ssrc = htonl(packet.sender_ssrc);
    packet.timestamp[0] = htonll(packet.timestamp[0]);
    packet.timestamp[1] = htonll(packet.timestamp[1]);
    packet.timestamp[2] = htonll(packet.timestamp[2]);
}
void swap_bytes(midi_packet_header_t &packet)
{
    packet.sequence_number = htons(packet.sequence_number);
    packet.timestamp = htonl(packet.timestamp);
    packet.sender_ssrc = htonl(packet.sender_ssrc);
}

/* Match exchange_packet_t equality */
MATCHER_P(ExchangeEquals, expected, "")
{
    return arg.command_header.signature == expected.command_header.signature &&
           arg.command_header.command == expected.command_header.command     &&
           arg.protocol_version == expected.protocol_version                 &&
           arg.initiator_token == expected.initiator_token                   &&
           arg.sender_ssrc == expected.sender_ssrc                           &&
           (0 == strcmp(arg.name, expected.name));
}

/* Match synchronisation_packet_t equality */
MATCHER_P(TimestampEquals, expected, "")
{
    /* Only first two timestamps are important in participant response */
    return arg.command_header.signature == expected.command_header.signature &&
           arg.command_header.command == expected.command_header.command     &&
           arg.sender_ssrc == expected.sender_ssrc                           &&
           arg.count == expected.count                                       &&
           arg.timestamp[SYNC0] == expected.timestamp[SYNC0]                 &&
           arg.timestamp[SYNC1] == expected.timestamp[SYNC1];
}

/* Match Array packet equality */
MATCHER_P2(HasBytes, bytes, size, "")
{
    return (memcmp(arg, bytes, size) == 0);
}

/* Action to set recvfrom data parameter */
ACTION_TEMPLATE(SetArg1ToValue,
        HAS_1_TEMPLATE_PARAMS(typename, T),
        AND_1_VALUE_PARAMS(value))
{
    *reinterpret_cast<T *>(arg1) = value;
}

/* Action to save sendto data to pointer */
ACTION_TEMPLATE(SaveArg1Value,
        HAS_1_TEMPLATE_PARAMS(typename, T),
        AND_1_VALUE_PARAMS(pointer))
{
    *pointer = *(reinterpret_cast<const T *>(arg1));
}

class NetworkInterfaceMock : public NetworkInterface {
public:
    MOCK_METHOD(nsapi_error_t, connect, (), (override));
    MOCK_METHOD(nsapi_error_t, disconnect, (), (override));
    MOCK_METHOD(NetworkStack*, get_stack, (), (override));
};

class UDPSocketMock : public UDPSocket {
public:
    MOCK_METHOD(nsapi_error_t, open, (NetworkStack*), (override));
    MOCK_METHOD(nsapi_size_or_error_t, recvfrom, (SocketAddress*, void*, nsapi_size_t), (override));
    MOCK_METHOD(nsapi_size_or_error_t, sendto, (const SocketAddress&, const void*, nsapi_size_t), (override));
    MOCK_METHOD(nsapi_error_t, bind, (const SocketAddress&), (override));
};

class TestRTPMIDI : public testing::Test {
public:
    NetworkInterfaceMock net_mock;
    UDPSocketMock control_socket_mock;
    UDPSocketMock midi_socket_mock;
    RTPMIDI rtpmidi{TEST_SSRC, TEST_NAME, 0, false, &net_mock, &control_socket_mock, &midi_socket_mock};

    exchange_packet_t invitation {invitation_packet};
    exchange_packet_t expected_response {expected_response_packet};
    synchronization_packet_t sync_0 {sync_packet_0};
    synchronization_packet_t sync_1 {sync_packet_1};
    synchronization_packet_t sync_2 {sync_packet_2};

    midi_packet_header_t midi_header = midi_packet_header;
    uint8_t midi_packet[size_of_midi_packet];
    MIDIMessage midi_message = MIDIMessage::NoteOn(48);

    static constexpr uint16_t CONTROL_PORT{5004};
    static constexpr uint16_t MIDI_PORT{5005};

    void SetUp() override
    {
        // Network Byte Ordering
        swap_bytes(invitation);
        swap_bytes(expected_response);
        swap_bytes(sync_0);
        swap_bytes(sync_1);
        swap_bytes(sync_2);
        swap_bytes(midi_header);

        const auto *ptr = reinterpret_cast<const uint8_t *>(&midi_header);
        size_t i = 0;
        for (; i < sizeof(midi_packet_header_t); ++i) {
            midi_packet[i] = *ptr++;
        }
        midi_packet[i++] = (BJZP | 1U); /* Default BJZP and one midi message */
        midi_packet[i++] = 0;
        midi_packet[i++] = midi_message.data[1];
        midi_packet[i++] = midi_message.data[2];
        midi_packet[i++] = midi_message.data[3];
    }
};

TEST_F(TestRTPMIDI, ConnectsNetworkInterfaceToReceiveSessionInvitation)
{
    EXPECT_CALL(net_mock, connect());

    auto error = rtpmidi.connect(CONTROL_PORT);

    ASSERT_THAT(error, Eq(RTPMIDI_ERROR_OK));
}

TEST_F(TestRTPMIDI, ErrorIfNetworkInterfaceUnavailable)
{
    RTPMIDI rtpmidi_with_null_interface{TEST_SSRC, TEST_NAME, nullptr,
        &control_socket_mock, &midi_socket_mock};

    auto error = rtpmidi_with_null_interface.connect(CONTROL_PORT);

    ASSERT_THAT(error, RTPMIDI_ERROR_CONNECT);
}

TEST_F(TestRTPMIDI, ErrorIfNetworkInterfaceReturnsError)
{
    EXPECT_CALL(net_mock, connect())
        .WillOnce(Return(NSAPI_ERROR_CONNECTION_TIMEOUT));

    auto error = rtpmidi.connect(CONTROL_PORT);

    ASSERT_THAT(error, Eq(RTPMIDI_ERROR_CONNECT));
}


TEST_F(TestRTPMIDI, PerformsConnectionHandshake)
{
    exchange_packet_t control_port_response;
    exchange_packet_t midi_port_response;
    SocketAddress control_port_address;
    SocketAddress midi_port_address;

    Sequence s1, s2, s3, s4;
    EXPECT_CALL(net_mock, connect())
        .InSequence(s1, s2, s3).WillOnce(Return(NSAPI_ERROR_OK));

    EXPECT_CALL(control_socket_mock, open(_)).InSequence(s2);
    EXPECT_CALL(midi_socket_mock, open(_)).InSequence(s3);

    EXPECT_CALL(control_socket_mock, bind)
        .InSequence(s2)
        .WillOnce(DoAll(SaveArg<0>(&control_port_address), Return(NSAPI_ERROR_OK)));

    EXPECT_CALL(midi_socket_mock, bind)
        .InSequence(s3)
        .WillOnce(DoAll(SaveArg<0>(&midi_port_address), Return(NSAPI_ERROR_OK)));

    EXPECT_CALL(control_socket_mock, recvfrom(NotNull(), NotNull(), sizeof(exchange_packet_t)))
        .InSequence(s1, s2)
        .WillOnce(DoAll(SetArg1ToValue<exchange_packet_t>(invitation), Return(NSAPI_ERROR_OK)));

    EXPECT_CALL(control_socket_mock, sendto(_, NotNull(), sizeof(exchange_packet_t)))
        .InSequence(s1)
        .WillOnce(DoAll(SaveArg1Value<exchange_packet_t>(&control_port_response), Return(NSAPI_ERROR_OK)));

    EXPECT_CALL(midi_socket_mock, recvfrom(NotNull(), NotNull(), sizeof(exchange_packet_t)))
        .InSequence(s1, s3)
        .WillOnce(DoAll(SetArg1ToValue<exchange_packet_t>(invitation), Return(NSAPI_ERROR_OK)));

    EXPECT_CALL(midi_socket_mock, sendto(_, NotNull(), sizeof(exchange_packet_t)))
        .InSequence(s1)
        .WillOnce(DoAll(SaveArg1Value<exchange_packet_t>(&midi_port_response), Return(NSAPI_ERROR_OK)));

    rtpmidi.connect(CONTROL_PORT);

    ASSERT_THAT(control_port_response, ExchangeEquals(expected_response));
    ASSERT_THAT(midi_port_response, ExchangeEquals(expected_response));
    ASSERT_THAT(control_port_address.get_port(), Eq(CONTROL_PORT));
    ASSERT_THAT(midi_port_address.get_port(), Eq(MIDI_PORT));
}

TEST_F(TestRTPMIDI, PerformsSynchronisationHandshake)
{
    synchronization_packet_t sync_response;

    Sequence s1;
    EXPECT_CALL(midi_socket_mock, recvfrom(NotNull(), NotNull(), sizeof(synchronization_packet_t)))
        .InSequence(s1)
        .WillOnce(DoAll(SetArg1ToValue<synchronization_packet_t>(sync_0), Return(NSAPI_ERROR_OK)));
    EXPECT_CALL(midi_socket_mock, sendto(_, NotNull(), sizeof(synchronization_packet_t)))
        .InSequence(s1)
        .WillOnce(DoAll(SaveArg1Value<synchronization_packet_t>(&sync_response), Return(NSAPI_ERROR_OK)));
    EXPECT_CALL(midi_socket_mock, recvfrom(NotNull(), NotNull(), sizeof(synchronization_packet_t)))
        .InSequence(s1)
        .WillOnce(DoAll(SetArg1ToValue<synchronization_packet_t>(sync_2), Return(NSAPI_ERROR_OK)));

    rtpmidi.synchronise();

    ASSERT_THAT(sync_response, TimestampEquals(sync_1));
}

TEST_F(TestRTPMIDI, SendsWrittenMIDIMessages)
{
    EXPECT_CALL(midi_socket_mock, sendto(_, HasBytes(midi_packet, size_of_midi_packet), size_of_midi_packet))
        .WillOnce(Return(NSAPI_ERROR_OK));

    rtpmidi.write(MIDIMessage::NoteOn(48));
    rtpmidi.send_midi_buffer();
}
