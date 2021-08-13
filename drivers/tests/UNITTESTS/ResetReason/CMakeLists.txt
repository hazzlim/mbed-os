# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

set(TEST_NAME resetreason-unittest)

add_executable(${TEST_NAME})

target_compile_definitions(${TEST_NAME}
    PRIVATE
        DEVICE_RESETREASON
)

target_sources(${TEST_NAME}
    PRIVATE
        ${mbed-os_SOURCE_DIR}/drivers/source/ResetReason.cpp
        test_resetreason.cpp
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        mbed-headers-drivers
)

gtest_discover_tests(${TEST_NAME} PROPERTIES LABELS "drivers")
