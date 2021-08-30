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
#include "rtos/Thread.h"
#include "events/EventQueue.h"
#include "platform/NonCopyable.h"
#include "MIDIMessage.h"
using namespace std::chrono_literals;

/* TODO: Remove this workaround */
#ifndef UNITTEST
#include "LWIPStack.h"
#if BYTE_ORDER == LITTLE_ENDIAN
    #define htonll(x) ((((uint64_t)htonl(x)) << 32) + htonl((x) >> 32))
#else
    #define htonll(x) ((uint64_t)x)
#endif /* BYTE_ORDER == LITTLE_ENDIAN */
#endif

enum apple_midi_defs : uint32_t {
    SIGNATURE        =  0xFFFFU,
    PROTOCOL_VERSION =  2U,
    VPXCC            =  0x80U,
    MPAYLOAD         =  0x61U,
    BJZP             =  0b00100000U
};


enum rtpmidi_command : uint32_t {
    INV              =  0x494EU,
    ACCEPT_INV       =  0x4F4BU,
    CK               =  0x434BU,
    SYNC0            =  0U,
    SYNC1            =  1U,
    SYNC2            =  2U
};

/* TODO: Find non-overlapping error range */
enum rtpmidi_error_t : int32_t {
    RTPMIDI_ERROR_OK        = 0,
    RTPMIDI_ERROR_CONNECT   = -9001,
    RTPMIDI_ERROR_COMMAND   = -9002,
    RTPMIDI_ERROR_SYNC      = -9003
};

/* Command Header */
typedef struct __attribute__((packed)) {
    uint16_t signature {SIGNATURE};
    uint16_t command;
} command_header_t;

/* Session Exchange Packet */
typedef struct __attribute__((packed)) {
    command_header_t command_header;
    uint32_t protocol_version {PROTOCOL_VERSION};
    uint32_t initiator_token;
    uint32_t sender_ssrc;
    char name[32];
} exchange_packet_t;

/* Synchronization Packet */
typedef struct __attribute__((packed)) {
    command_header_t command_header;
    uint32_t sender_ssrc;
    uint8_t count;
    uint8_t padding[3] {0, 0, 0};
    uint64_t timestamp[3];
} synchronization_packet_t;

/* MIDI Packet Header */
typedef struct __attribute__((packed)) {
    uint8_t vpxcc {VPXCC};
    uint8_t mpayload {MPAYLOAD};
    uint16_t sequence_number;
    uint32_t timestamp;
    uint32_t sender_ssrc;
} midi_packet_header_t;

/* Constant from MIDIMessage.h */
constexpr size_t MAX_MIDI_COMMAND_LEN = MAX_MIDI_MESSAGE_SIZE;

/* Number of preliminary synchronization calls required - based on testing */
constexpr size_t INITIAL_SYNC_COUNT = 6;
constexpr int SOCKET_TIMEOUT = 100;
constexpr std::chrono::seconds SYNC_FREQUENCY{10s};

class RTPMIDI : private mbed::NonCopyable<RTPMIDI> {
public:
    RTPMIDI() = delete;

    RTPMIDI(uint32_t ssrc,
            std::string &&name,
            uint16_t port = 5004,
            bool connect_immediately = true,
            NetworkInterface *net = NetworkInterface::get_default_instance(),
            UDPSocket *control_socket = new UDPSocket,
            UDPSocket *midi_socket = new UDPSocket);

    ~RTPMIDI();


    rtpmidi_error_t connect_and_sync(uint16_t control_port);
    rtpmidi_error_t connect(uint16_t control_port);
    void synchronise();
    void write(MIDIMessage msg);
    void send_midi_buffer();
private:
    uint32_t _ssrc;
    std::string _name;
    rtos::Thread _rtp_thread;
    events::EventQueue _rtp_queue{32 * EVENTS_EVENT_SIZE};

    static constexpr size_t CONTROL_SOCKET = 0;
    static constexpr size_t MIDI_SOCKET = 1;
    NetworkInterface *_net;
    UDPSocket *_sockets[2];
    SocketAddress _peer_address;

    static constexpr size_t MidiCommandHeaderPos = sizeof(midi_packet_header_t);
    static constexpr size_t MidiHeaderLength = MidiCommandHeaderPos + 1; /* Command Header Byte */
    static constexpr size_t MaxMidiPacketSize = MidiHeaderLength + MAX_MIDI_COMMAND_LEN;
    static constexpr size_t StartOfMidiMessages = MidiHeaderLength;
    uint8_t midi_buffer[MaxMidiPacketSize];
    size_t midi_buffer_pos = StartOfMidiMessages;
    size_t midi_buffer_size = 0;
    uint16_t sequence_number = 0;

    bool connect_to_network();
    void exchange_handshake(uint16_t port, size_t SOCKET);
    exchange_packet_t exchange_response(const exchange_packet_t &invitation);
    uint64_t get_current_timestamp();
};

#endif // RTP_MIDI_H
