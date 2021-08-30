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
#include "rtos/Kernel.h"
#include "events/Event.h"
#include <cstdio>

RTPMIDI::RTPMIDI(uint32_t ssrc,
                 std::string &&name,
                 uint16_t port,
                 bool connect_immediately,
                 NetworkInterface *net,
                 UDPSocket *control_socket,
                 UDPSocket *midi_socket)
    : _ssrc{ssrc}, _name{name}, _net{net}, _sockets{control_socket, midi_socket}
{
    if (connect_immediately) {
        connect_and_sync(port);
    }

}

RTPMIDI::~RTPMIDI()
{
    if (_net) {
        _net->disconnect();
    }
    if (_sockets[CONTROL_SOCKET]) {
        delete _sockets[CONTROL_SOCKET];
    }
    if (_sockets[MIDI_SOCKET]) {
        delete _sockets[MIDI_SOCKET];
    }
}

rtpmidi_error_t RTPMIDI::connect_and_sync(uint16_t control_port)
{
    auto error = connect(control_port);
    if (error == RTPMIDI_ERROR_OK) {

        for (size_t i = 0; i < INITIAL_SYNC_COUNT; ++i) {
            synchronise();
        }

        /* Configure timeout */
        _sockets[MIDI_SOCKET]->set_timeout(SOCKET_TIMEOUT);

        events::Event<void()> sync_event = _rtp_queue.event(this, &RTPMIDI::synchronise);
        sync_event.period(SYNC_FREQUENCY);
        sync_event.post();
        _rtp_thread.start(callback(&_rtp_queue, &events::EventQueue::dispatch_forever));

    }

    return error;
}

rtpmidi_error_t RTPMIDI::connect(uint16_t control_port)
{
    printf("Try to connect\n");
    if(!connect_to_network()) {
        return RTPMIDI_ERROR_CONNECT;
    }

    printf("Start handshake\n");
    exchange_handshake(control_port, CONTROL_SOCKET);
    exchange_handshake(control_port + 1, MIDI_SOCKET);

    return RTPMIDI_ERROR_OK;
}

void RTPMIDI::exchange_handshake(uint16_t port, size_t SOCKET)
{
    SocketAddress bind_address;
    bind_address.set_port(port);
    _sockets[SOCKET]->bind(bind_address);

    exchange_packet_t invitation_packet;
    exchange_packet_t response_packet;
/* DEBUG */
    printf("Waiting to receive\n");
/* DEBUG */
    _sockets[SOCKET]->recvfrom(&_peer_address, &invitation_packet, sizeof(exchange_packet_t));

    response_packet = exchange_response(invitation_packet);

/* DEBUG */
    printf("Send response\n");
/* DEBUG */
    _sockets[SOCKET]->sendto(_peer_address, &response_packet, sizeof(exchange_packet_t));
}

exchange_packet_t RTPMIDI::exchange_response(const exchange_packet_t &invitation)
{
    exchange_packet_t response;
    response.command_header.command = htons(ACCEPT_INV);
    response.protocol_version = htonl(PROTOCOL_VERSION);
    response.initiator_token = invitation.initiator_token;
    response.sender_ssrc = htonl(_ssrc);
    strcpy(response.name, _name.c_str());
    return response;
}

bool RTPMIDI::connect_to_network()
{
    if(!_net || !_sockets[CONTROL_SOCKET] || !_sockets[MIDI_SOCKET]) {
        return false;
    }

    auto error = _net->connect();
    if(error != NSAPI_ERROR_OK) {
        return false;
    }
/* DEBUG */
    SocketAddress sockAddr;
    _net->get_ip_address(&sockAddr);
    printf("IP address is: %s\n", sockAddr.get_ip_address() ? sockAddr.get_ip_address() : "No IP");
/* DEBUG */

    error = _sockets[CONTROL_SOCKET]->open(_net);
    if(error != NSAPI_ERROR_OK) {
        return false;
    }
    error = _sockets[MIDI_SOCKET]->open(_net);
    if(error != NSAPI_ERROR_OK) {
        return false;
    }

    return true;
}

void RTPMIDI::synchronise()
{
    SocketAddress sockAddr;
    synchronization_packet_t sync_0;
    synchronization_packet_t sync_1;
    synchronization_packet_t sync_2;

    _sockets[MIDI_SOCKET]->recvfrom(&sockAddr, &sync_0, sizeof(synchronization_packet_t));

    sync_1.command_header.command = htons(CK);
    sync_1.sender_ssrc = htonl(_ssrc);
    sync_1.count = SYNC1;
    sync_1.timestamp[SYNC0] = sync_0.timestamp[SYNC0];
    sync_1.timestamp[SYNC1] = htonll(get_current_timestamp());

    _sockets[MIDI_SOCKET]->sendto(sockAddr, &sync_1, sizeof(synchronization_packet_t));
    _sockets[MIDI_SOCKET]->recvfrom(&sockAddr, &sync_2, sizeof(synchronization_packet_t));

    /* Calculate time drift */
}

uint64_t RTPMIDI::get_current_timestamp()
{
    auto now = rtos::Kernel::Clock::now();
    auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    auto timestamp_us = now_us.time_since_epoch();
    /* Return in units of 100us */
    return timestamp_us.count() / 100;
}

void RTPMIDI::write(MIDIMessage msg)
{
    /* Check if we need to clear the buffer */
    if (midi_buffer_size + msg.length > MAX_MIDI_COMMAND_LEN) {
        send_midi_buffer();
    }

    /* Replace first byte (CN) with relative timestamp */
    midi_buffer[midi_buffer_pos++] = 0x0;
    for (int i = 1; i < msg.length; ++i) {
        midi_buffer[midi_buffer_pos++] = msg.data[i];
    }
    midi_buffer_size += msg.length;
}

void RTPMIDI::send_midi_buffer()
{
    /* Set Midi Packet Header */
    midi_packet_header_t *header_p = reinterpret_cast<midi_packet_header_t *>(midi_buffer);
    header_p->vpxcc = VPXCC;
    header_p->mpayload = MPAYLOAD;
    header_p->sequence_number = htons(sequence_number++);

    auto timestamp_64 = get_current_timestamp();
    header_p->timestamp = htonl(static_cast<uint32_t>(timestamp_64));

    header_p->sender_ssrc = htonl(_ssrc);

    /* Set Command Header */
    midi_buffer[MidiCommandHeaderPos] = (BJZP | midi_buffer_size);

    /* Send the buffer */
    _sockets[MIDI_SOCKET]->sendto(_peer_address, midi_buffer, MidiHeaderLength + midi_buffer_size);

    /* Reset Buffer */
    midi_buffer_pos = StartOfMidiMessages;
    midi_buffer_size = 0;
}
