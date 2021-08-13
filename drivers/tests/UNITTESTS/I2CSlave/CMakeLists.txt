# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

set(TEST_NAME i2cslave-unittest)

add_executable(${TEST_NAME})

target_compile_definitions(${TEST_NAME}
    PRIVATE
        DEVICE_I2C
        DEVICE_I2CSLAVE
)

target_sources(${TEST_NAME}
    PRIVATE
    ${mbed-os_SOURCE_DIR}/drivers/source/I2CSlave.cpp
        test_i2cslave.cpp
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        mbed-headers-platform
        mbed-headers-hal
        mbed-headers-drivers
        mbed-stubs-hal
)

gtest_discover_tests(${TEST_NAME} PROPERTIES LABELS "drivers")
