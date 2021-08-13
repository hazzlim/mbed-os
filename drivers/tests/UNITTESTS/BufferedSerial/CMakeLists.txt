# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

set(TEST_NAME bufferedserial-unittest)

add_executable(${TEST_NAME})

target_compile_definitions(${TEST_NAME}
    PRIVATE
        DEVICE_SERIAL
        DEVICE_INTERRUPTIN
        MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE=115200
)

target_sources(${TEST_NAME}
    PRIVATE
        ${mbed-os_SOURCE_DIR}/drivers/source/BufferedSerial.cpp
        test_bufferedserial.cpp
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        mbed-headers-platform
        mbed-headers-hal
        mbed-headers-drivers
        mbed-stubs-platform
        mbed-stubs-drivers
)

gtest_discover_tests(${TEST_NAME} PROPERTIES LABELS "drivers")
