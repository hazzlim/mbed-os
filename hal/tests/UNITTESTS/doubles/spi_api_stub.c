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

#include "hal/spi_api.h"

#if DEVICE_SPI

#ifdef DEVICE_SPI_COUNT
SPIName spi_get_peripheral_name(PinName mosi, PinName miso, PinName mclk)
{
    return (SPIName)NC;

}
#endif // DEVICE_SPI_COUNT

void spi_get_capabilities(PinName ssel, bool slave, spi_capabilities_t *cap)
{
}

void spi_init_direct(spi_t *obj, const spi_pinmap_t *pinmap)
{
}

void spi_init(spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel)
{
}

void spi_free(spi_t *obj)
{
}

void spi_format(spi_t *obj, int bits, int mode, int slave)
{
}

void spi_frequency(spi_t *obj, int hz)
{
}

int  spi_master_write(spi_t *obj, int value)
{
    return 0;
}

int spi_master_block_write(spi_t *obj, const char *tx_buffer, int tx_length, char *rx_buffer, int rx_length, char write_fill)
{
    return 0;
}

int  spi_slave_receive(spi_t *obj)
{
    return 0;
}

int  spi_slave_read(spi_t *obj)
{
    return 0;
}

void spi_slave_write(spi_t *obj, int value)
{
}

int  spi_busy(spi_t *obj)
{
    return 0;
}

uint8_t spi_get_module(spi_t *obj)
{
    return 0;
}

const PinMap *spi_master_mosi_pinmap(void)
{
    return NULL;
}

const PinMap *spi_master_miso_pinmap(void)
{
    return NULL;
}

const PinMap *spi_master_clk_pinmap(void)
{
    return NULL;
}

const PinMap *spi_master_cs_pinmap(void)
{
    return NULL;
}

const PinMap *spi_slave_mosi_pinmap(void)
{
    return NULL;
}

const PinMap *spi_slave_miso_pinmap(void)
{
    return NULL;
}

const PinMap *spi_slave_clk_pinmap(void)
{
    return NULL;
}

const PinMap *spi_slave_cs_pinmap(void)
{
    return NULL;
}

#if DEVICE_SPI_ASYNCH
void spi_master_transfer(spi_t *obj, const void *tx, size_t tx_length, void *rx, size_t rx_length, uint8_t bit_width, uint32_t handler, uint32_t event, DMAUsage hint)
{
}

uint32_t spi_irq_handler_asynch(spi_t *obj)
{
    return 0;
}

uint8_t spi_active(spi_t *obj)
{
    return 0;
}
void spi_abort_asynch(spi_t *obj)
{
}
#endif // DEVICE_SPI_ASYNCH

#endif // DEVICE_SPI
