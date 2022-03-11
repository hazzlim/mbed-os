/*
 * Copyright (c) 2022 Arm Limited and affiliates.
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
#include "hal/ticker_api.h"

void ticker_set_handler(const ticker_data_t *const ticker, ticker_event_handler handler)
{
}

void ticker_remove_event(const ticker_data_t *const ticker, ticker_event_t *obj)
{
}

void ticker_insert_event(const ticker_data_t *const ticker, ticker_event_t *obj, timestamp_t timestamp, uintptr_t id)
{
}

void ticker_insert_event_us(const ticker_data_t *const ticker, ticker_event_t *obj, us_timestamp_t timestamp, uintptr_t id)
{
}

us_timestamp_t ticker_read_us(const ticker_data_t *const ticker)
{
    return 0;
}
