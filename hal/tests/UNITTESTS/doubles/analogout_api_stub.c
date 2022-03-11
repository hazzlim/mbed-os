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

#include "hal/analogout_api.h"
#include <stddef.h>

#if DEVICE_ANALOGOUT

void analogout_init_direct(dac_t *obj, const PinMap *pinmap)
{
}

void analogout_init(dac_t *obj, PinName pin)
{
}

void analogout_free(dac_t *obj)
{
}

void analogout_write(dac_t *obj, float value)
{
}

void analogout_write_u16(dac_t *obj, uint16_t value)
{
}

float analogout_read(dac_t *obj)
{
    return 0;
}

uint16_t analogout_read_u16(dac_t *obj)
{
    return 0;
}

const PinMap *analogout_pinmap(void)
{
    return NULL;
}

#endif // DEVICE_ANALOGOUT
