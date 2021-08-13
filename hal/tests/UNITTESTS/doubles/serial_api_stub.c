/*
 * Copyright (c) 2021 Arm Limited and affiliates.
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

#include "hal/serial_api.h"

#if DEVICE_SERIAL

void serial_init(serial_t *obj, PinName tx, PinName rx)
{
}

void serial_init_direct(serial_t *obj, const serial_pinmap_t *pinmap)
{
}

void serial_free(serial_t *obj)
{
}

void serial_baud(serial_t *obj, int baudrate)
{
}

void serial_format(serial_t *obj, int data_bits, SerialParity parity, int stop_bits)
{
}

void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uintptr_t context)
{
}

void serial_irq_set(serial_t *obj, SerialIrq irq, uint32_t enable)
{
}

int  serial_getc(serial_t *obj)
{
    return 0;
}

void serial_putc(serial_t *obj, int c)
{
}

int  serial_readable(serial_t *obj)
{
    return 0;
}

int  serial_writable(serial_t *obj)
{
    return 0;
}

void serial_clear(serial_t *obj)
{
}

void serial_break_set(serial_t *obj)
{
}

void serial_break_clear(serial_t *obj)
{
}

void serial_pinout_tx(PinName tx)
{
}

#if DEVICE_SERIAL_FC
void serial_set_flow_control(serial_t *obj, FlowControl type, PinName rxflow, PinName txflow)
{
}

void serial_set_flow_control_direct(serial_t *obj, FlowControl type, const serial_fc_pinmap_t *pinmap)
{
}
#endif // DEVICE_SERIAL_FC

const PinMap *serial_tx_pinmap(void)
{
    return NULL;
}

const PinMap *serial_rx_pinmap(void)
{
    return NULL;
}

#if DEVICE_SERIAL_FC
const PinMap *serial_cts_pinmap(void)
{
    return NULL;
}

const PinMap *serial_rts_pinmap(void)
{
    return NULL;
}
#endif // DEVICE_SERIAL_FC

#if DEVICE_SERIAL_ASYNCH
int serial_tx_asynch(serial_t *obj, const void *tx, size_t tx_length, uint8_t tx_width, uint32_t handler, uint32_t event, DMAUsage hint)
{
    return 0;
}

void serial_rx_asynch(serial_t *obj, void *rx, size_t rx_length, uint8_t rx_width, uint32_t handler, uint32_t event, uint8_t char_match, DMAUsage hint)
{
}

uint8_t serial_tx_active(serial_t *obj)
{
    return 0;
}

uint8_t serial_rx_active(serial_t *obj)
{
    return 0;
}

int serial_irq_handler_asynch(serial_t *obj)
{
    return 0;
}

void serial_tx_abort_asynch(serial_t *obj)
{
}

void serial_rx_abort_asynch(serial_t *obj)
{
}
#endif // DEVICE_SERIAL_ASYNCH

#endif // DEVICE_SERIAL
