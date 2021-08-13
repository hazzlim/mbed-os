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

#include "hal/gpio_api.h"

#if DEVICE_INTERRUPTIN

int gpio_is_connected(const gpio_t *obj)
{
    return 0;
}

void gpio_free(gpio_t *obj)
{
}

void gpio_mode(gpio_t *obj, PinMode mode)
{
}

void gpio_dir(gpio_t *obj, PinDirection direction)
{
}

void gpio_write(gpio_t *obj, int value)
{
}

int gpio_read(gpio_t *obj)
{
    return 0;
}

void gpio_init_in(gpio_t *gpio, PinName pin)
{
}

void gpio_init_in_ex(gpio_t *gpio, PinName pin, PinMode mode)
{
}

void gpio_init_out(gpio_t *gpio, PinName pin)
{
}

void gpio_init_out_ex(gpio_t *gpio, PinName pin, int value)
{
}

void gpio_init_inout(gpio_t *gpio, PinName pin, PinDirection direction, PinMode mode, int value)
{
}

#endif  // DEVICE_INTERRUPTIN
