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
#include "netsocket/UDPSocket.h"
#include "netsocket/NetworkInterface.h"
#include "LWIPStack.h"

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
enum rtpmidi_error_t : int32_t {
    RTPMIDI_ERROR_OK        = 0,
    RTPMIDI_ERROR_CONNECT   = -9001,
    RTPMIDI_ERROR_COMMAND   = -9002,
    RTPMIDI_ERROR_SYNC      = -9003
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

/* Byte Swapping Functions */
void to_network_order(exchange_packet_t &packet)
{
    packet.command_header.signature = htons(packet.command_header.signature);
    packet.command_header.command = htons(packet.command_header.command);
    packet.protocol_version = htonl(packet.protocol_version);
    packet.initiator_token = htonl(packet.initiator_token);
    packet.sender_ssrc = htonl(packet.sender_ssrc);
}

void to_host_order(exchange_packet_t &packet)
{
    to_network_order(packet);
}

class RTPMIDI {
public:
    RTPMIDI(NetworkInterface *net, UDPSocket *socket);

    rtpmidi_error_t connect();
private:
    NetworkInterface *_net;
    UDPSocket *_socket;

    bool connect_to_network();
    bool exchange_handshake();
};

#endif // RTP_MIDI_H
