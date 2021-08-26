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

RTPMIDI::RTPMIDI(NetworkInterface *net, UDPSocket *socket) : _net{net}, _socket{socket}
{
}

rtpmidi_error_t RTPMIDI::connect()
{
    if(!connect_to_network()) {
        return RTPMIDI_ERROR_CONNECT;
    }

    exchange_handshake();

    return RTPMIDI_ERROR_OK;
}

bool RTPMIDI::exchange_handshake()
{
    SocketAddress address;
    exchange_packet_t invitation_packet;

    _socket->recvfrom(&address, &invitation_packet, sizeof(invitation_packet));

    exchange_packet_t response_packet = {
        SIGNATURE,
        htons(ACCEPT_INV),
        htonl(PROTOCOL_VERSION),
        invitation_packet.initiator_token,
        htonl(0xdbffa3a1),
        "NAME"
    };

    _socket->sendto(address, &response_packet, sizeof(invitation_packet));
    _socket->recvfrom(&address, &invitation_packet, sizeof(invitation_packet));
    _socket->sendto(address, &response_packet, sizeof(invitation_packet));
}

bool RTPMIDI::connect_to_network()
{
    if(!_net || !_socket) {
        return false;
    }

    auto error = _net->connect();
    if(error != NSAPI_ERROR_OK) {
        return false;
    }

    error = _socket->open(_net);
    if(error != NSAPI_ERROR_OK) {
        return false;
    }

    return true;
}

