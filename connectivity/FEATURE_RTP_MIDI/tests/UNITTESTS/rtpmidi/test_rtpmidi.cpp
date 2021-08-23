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

#include "gtest/gtest.h"

class TestRTPMIDI : public testing::Test {
    public:
        RTPMIDI rtpmidi;
        exchange_packet_t invitation_packet;
        exchange_packet_t response_packet;
};

TEST_F(TestRTPMIDI, AcceptsInvitation)
{
    // Invitation Command
    invitation_packet.command_header.command = INV;
    auto error = rtpmidi.accept(invitation_packet, response_packet);

    ASSERT_EQ(error, RTPMIDI_ERROR_OK);
    ASSERT_EQ(response_packet.command_header.signature, SIGNATURE);
    ASSERT_EQ(response_packet.command_header.command, ACCEPT_INV);
    ASSERT_EQ(response_packet.protocol_version, PROTOCOL_VERSION);
    ASSERT_EQ(response_packet.initiator_token, invitation_packet.initiator_token);
    ASSERT_EQ(response_packet.sender_ssrc, rtpmidi.ssrc());
    ASSERT_EQ(response_packet.name, rtpmidi.name());

}

TEST_F(TestRTPMIDI, ErrorOnInvalidInvitation)
{
    // Invalid Command
    invitation_packet.command_header.command = 0;
    auto error = rtpmidi.accept(invitation_packet, response_packet);

    ASSERT_EQ(error, RTPMIDI_ERROR_COMMAND);
}

TEST_F(TestRTPMIDI, RespondsToSynchronizationPacket)
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
