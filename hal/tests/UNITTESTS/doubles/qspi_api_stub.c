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

#include "hal/qspi_api.h"

#if DEVICE_QSPI

qspi_status_t qspi_init(qspi_t *obj, PinName io0, PinName io1, PinName io2, PinName io3, PinName sclk, PinName ssel, uint32_t hz, uint8_t mode)
{
    return QSPI_STATUS_OK;
}

qspi_status_t qspi_init_direct(qspi_t *obj, const qspi_pinmap_t *pinmap, uint32_t hz, uint8_t mode)
{
    return QSPI_STATUS_OK;
}

qspi_status_t qspi_free(qspi_t *obj)
{
    return QSPI_STATUS_OK;
}

qspi_status_t qspi_frequency(qspi_t *obj, int hz)
{
    return QSPI_STATUS_OK;
}

qspi_status_t qspi_write(qspi_t *obj, const qspi_command_t *command, const void *data, size_t *length)
{
    return QSPI_STATUS_OK;
}

qspi_status_t qspi_command_transfer(qspi_t *obj, const qspi_command_t *command, const void *tx_data, size_t tx_size, void *rx_data, size_t rx_size)
{
    return QSPI_STATUS_OK;
}

qspi_status_t qspi_read(qspi_t *obj, const qspi_command_t *command, void *data, size_t *length)
{
    return QSPI_STATUS_OK;
}

const PinMap *qspi_master_sclk_pinmap(void)
{
    return NULL;
}

const PinMap *qspi_master_ssel_pinmap(void)
{
    return NULL;
}

const PinMap *qspi_master_data0_pinmap(void)
{
    return NULL;
}

const PinMap *qspi_master_data1_pinmap(void)
{
    return NULL;
}

const PinMap *qspi_master_data2_pinmap(void)
{
    return NULL;
}

const PinMap *qspi_master_data3_pinmap(void)
{
    return NULL;
}

#endif // DEVICE_QSPI
