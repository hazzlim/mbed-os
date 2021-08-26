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

MATCHER_P(Equals, expected, "")
{
    return arg.command_header.signature == expected.command_header.signature &&
           arg.command_header.command == expected.command_header.command     &&
           arg.protocol_version == expected.protocol_version                 &&
           arg.initiator_token == expected.initiator_token                   &&
           arg.sender_ssrc == expected.sender_ssrc                           &&
           (0 == strcmp(arg.name, expected.name));
}

class NetworkInterfaceMock : public NetworkInterface {
public:
    MOCK_METHOD(nsapi_error_t, connect, (), (override));
    MOCK_METHOD(nsapi_error_t, disconnect, (), (override));
    MOCK_METHOD(NetworkStack*, get_stack, (), (override));
};


/* Action to set recvfrom data parameter */
ACTION_P(SetArg1ToValue, value) { *reinterpret_cast<exchange_packet_t *>(arg1) = value; }

class UDPSocketMock : public UDPSocket {
public:
    MOCK_METHOD(nsapi_error_t, open, (NetworkStack*), (override));
    MOCK_METHOD(nsapi_size_or_error_t, recvfrom, (SocketAddress*, void*, nsapi_size_t), (override));
    MOCK_METHOD(nsapi_size_or_error_t, sendto, (const SocketAddress&, const void*, nsapi_size_t), (override));
};

class TestRTPMIDI : public testing::Test {
public:
    NetworkInterfaceMock net_mock;
    UDPSocketMock socket_mock;
    RTPMIDI rtpmidi{&net_mock, &socket_mock};

    exchange_packet_t invitation {invitation_packet};
};

TEST_F(TestRTPMIDI, ConnectsNetworkInterfaceToReceiveSessionInvitation)
{
    EXPECT_CALL(net_mock, connect());

    auto error = rtpmidi.connect();

    EXPECT_THAT(error, Eq(RTPMIDI_ERROR_OK));
}

TEST_F(TestRTPMIDI, ErrorIfNetworkInterfaceUnavailable)
{
    RTPMIDI rtpmidi_with_null_interface{nullptr, &socket_mock};

    auto error = rtpmidi_with_null_interface.connect();

    EXPECT_THAT(error, RTPMIDI_ERROR_CONNECT);
}

TEST_F(TestRTPMIDI, ErrorIfNetworkInterfaceReturnsError)
{
    EXPECT_CALL(net_mock, connect())
        .WillOnce(Return(NSAPI_ERROR_CONNECTION_TIMEOUT));

    auto error = rtpmidi.connect();

    EXPECT_THAT(error, Eq(RTPMIDI_ERROR_CONNECT));
}


TEST_F(TestRTPMIDI, RespondsToSessionInvitation)
{
    EXPECT_CALL(net_mock, connect())
        .WillOnce(Return(NSAPI_ERROR_OK));

    EXPECT_CALL(socket_mock, open(_));
    EXPECT_CALL(socket_mock, recvfrom(NotNull(), NotNull(), sizeof(exchange_packet_t)))
        .WillOnce(DoAll(SetArg1ToValue(invitation), Return(NSAPI_ERROR_OK)));

    // Mock SocketAddress so that we can check sent back to initiator, and on
    // correct port
    //EXPECT_CALL(socket_mock, sendto(_, NotNull(), sizeof(exchange_packet_t)));

    rtpmidi.connect();
}
