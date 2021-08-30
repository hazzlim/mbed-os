/*
 * Copyright (c) 2021, Arm Limited and affiliates.
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

#ifndef MIDI_READ_WRITE_H
#define MIDI_READ_WRITE_H

class MIDIReadWrite {
public:
    virtual ~MIDIReadWrite() = default;

    /**
     * Check if this class is ready
     *
     * @return true if configured, false otherwise
     */
    virtual bool ready() = 0;

    /**
     * Block until this device is configured
     */
    virtual void wait_ready() = 0;

    /**
     * Send a MIDIMessage
     *
     * @param m The MIDIMessage to send
     * @return true if the message was sent, false otherwise
     */
    virtual bool write(MIDIMessage m) = 0;

    /**
     * Check if a message can be read
     *
     * @return true if a packet can be read false otherwise
     */
    virtual bool readable() = 0;

    /**
     * Read a message
     *
     * @param m The MIDIMessage to fill
     * @return true if a message was read, false otherwise
     */
    virtual bool read(MIDIMessage *m) = 0;

    /**
     * Attach a callback for when a MIDIEvent is received
     *
     * @param callback code to call when a packet is received
     */
    virtual void attach(mbed::Callback<void()> callback) = 0;
};

#endif // MIDI_READ_WRITE_H
