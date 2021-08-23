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

#ifndef RTP_MIDI_H
#define RTP_MIDI_H

#include <string>
#include <cstring>

enum apple_midi_defs : uint32_t {
    SIGNATURE        =  0xFFFFU,
    PROTOCOL_VERSION =  2U,
    VPXCC            =  0x80U,
    MPAYLOAD         =  0x61U
};


enum rtpmidi_command : uint32_t {
    INV              =  0x494EU,
    ACCEPT_INV       =  0x4F4BU,
    CK               =  0x434BU,
    SYNC0            =  0U,
    SYNC1            =  1U,
};

/* TODO: Find non-overlapping error range */
enum rtpmidi_error : int32_t {
    RTPMIDI_ERROR_OK        = 0,
    RTPMIDI_ERROR_COMMAND   = -9001,
    RTPMIDI_ERROR_SYNC      = -9002
};

/* Command Header */
typedef struct {
    uint16_t signature {SIGNATURE};
    uint16_t command;
} command_header_t;

/* Session Exchange Packet */
typedef struct {
    command_header_t command_header;
    uint32_t protocol_version {PROTOCOL_VERSION};
    uint32_t initiator_token;
    uint32_t sender_ssrc;
    char name[32];
} exchange_packet_t;

/* Synchronization Packet */
typedef struct {
    command_header_t command_header;
    uint32_t sender_ssrc;
    uint8_t count;
    uint8_t padding[3];
    uint64_t timestamp[3];
} synchronization_packet_t;

/* MIDI Packet Header */
typedef struct {
    uint8_t vpxcc {VPXCC};
    uint8_t mpayload {MPAYLOAD};
    uint16_t sequence_number;
    uint32_t timestamp;
    uint32_t sender_ssrc;
} midi_packet_header_t;

class RTPMIDI {
public:
    rtpmidi_error accept(const exchange_packet_t &invitation, exchange_packet_t &response) const
    {
        if(invitation.command_header.command != INV) {
            return RTPMIDI_ERROR_COMMAND;
        }
        set_command_header(response.command_header, ACCEPT_INV);
        response.initiator_token = invitation.initiator_token;
        response.sender_ssrc = _ssrc;
        strcpy(response.name, _name.c_str());
        return RTPMIDI_ERROR_OK;
    }

    rtpmidi_error synchronization_response(const synchronization_packet_t &initiator,
                                           synchronization_packet_t &response,
                                           uint64_t response_timestamp)
    {
        if(initiator.count != SYNC0) {
            return RTPMIDI_ERROR_SYNC;
        }
        set_command_header(response.command_header, CK);
        response.sender_ssrc = _ssrc;
        response.count = SYNC1;
        response.timestamp[SYNC0] = initiator.timestamp[SYNC0];
        response.timestamp[SYNC1] = response_timestamp;
        return RTPMIDI_ERROR_OK;
    }

    void set_command_header(command_header_t &header, rtpmidi_command command) const
    {
        header.command = command;
    }

    void generate_midi_header(midi_packet_header_t &header, uint16_t sequence_number, uint32_t timestamp)
    {
        header.sequence_number = sequence_number;
        header.timestamp = timestamp;
        header.sender_ssrc = _ssrc;

    }

    uint32_t ssrc() const
    {
        return _ssrc;
    }

    std::string name() const
    {
        return _name;
    }

private:
    uint32_t _ssrc {};
    std::string _name = "mbed-rtp-midi";

};

#endif // RTP_MIDI_H
