# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

set(TEST_NAME busin-unittest)

add_executable(${TEST_NAME})

target_compile_definitions(${TEST_NAME}
    PRIVATE
)

target_sources(${TEST_NAME}
    PRIVATE
        ${mbed-os_SOURCE_DIR}/drivers/source/BusIn.cpp
        test_busin.cpp
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        mbed-headers-platform
        mbed-headers-drivers
        mbed-headers-hal
        mbed-stubs-platform
        mbed-stubs-drivers
        mbed-stubs-hal
)

gtest_discover_tests(${TEST_NAME} PROPERTIES LABELS "drivers")
