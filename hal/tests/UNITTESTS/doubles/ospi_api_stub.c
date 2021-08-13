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

#include "hal/ospi_api.h"

#if DEVICE_OSPI

ospi_status_t ospi_command_transfer(ospi_t *obj, const ospi_command_t *command, const void *tx_data, size_t tx_size, void *rx_data, size_t rx_size)
{
    return OSPI_STATUS_OK;
}

ospi_status_t ospi_frequency(ospi_t *obj, int hz)
{
    return OSPI_STATUS_OK;
}

ospi_status_t ospi_init(ospi_t *obj, PinName io0, PinName io1, PinName io2, PinName io3, PinName io4, PinName io5, PinName io6, PinName io7,
                        PinName sclk, PinName ssel, PinName dqs, uint32_t hz, uint8_t mode)
{
    return OSPI_STATUS_OK;
}

ospi_status_t ospi_init_direct(ospi_t *obj, const ospi_pinmap_t *pinmap, uint32_t hz, uint8_t mode)
{
    return OSPI_STATUS_OK;
}

ospi_status_t ospi_read(ospi_t *obj, const ospi_command_t *command, void *data, size_t *length)
{
    return OSPI_STATUS_OK;
}

ospi_status_t ospi_write(ospi_t *obj, const ospi_command_t *command, const void *data, size_t *length)
{
    return OSPI_STATUS_OK;
}

#endif // DEVICE_OSPI
