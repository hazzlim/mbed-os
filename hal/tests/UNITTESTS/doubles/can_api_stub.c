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

#include "hal/can_api.h"

#if DEVICE_CAN

void can_init(can_t *obj, PinName rd, PinName td)
{
}

void can_init_direct(can_t *obj, const can_pinmap_t *pinmap)
{
}

void can_init_freq(can_t *obj, PinName rd, PinName td, int hz)
{
}

void can_init_freq_direct(can_t *obj, const can_pinmap_t *pinmap, int hz)
{
}

void can_free(can_t *obj)
{
}

int can_frequency(can_t *obj, int hz)
{
    return 0;
}

void can_irq_init(can_t *obj, can_irq_handler handler, uintptr_t id)
{
}

void can_irq_free(can_t *obj)
{
}

void can_irq_set(can_t *obj, CanIrqType irq, uint32_t enable)
{
}

int can_write(can_t *obj, CAN_Message msg, int cc)
{
    return 0;
}

int can_read(can_t *obj, CAN_Message *msg, int handle)
{
    return 0;
}

int can_mode(can_t *obj, CanMode mode)
{
    return 0;
}

int can_filter(can_t *obj, uint32_t id, uint32_t mask, CANFormat format, int32_t handle)
{
    return 0;
}

void can_reset(can_t *obj)
{
}

unsigned char can_rderror(can_t *obj)
{
    return 0;
}

unsigned char can_tderror(can_t *obj)
{
    return 0;
}

void can_monitor(can_t *obj, int silent)
{
}

#endif // DEVICE_CAN
