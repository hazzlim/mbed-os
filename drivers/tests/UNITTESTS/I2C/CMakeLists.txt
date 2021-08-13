# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

set(TEST_NAME i2c-unittest)

add_executable(${TEST_NAME})

target_compile_definitions(${TEST_NAME}
    PRIVATE
        DEVICE_I2C
        DEVICE_I2C_ASYNCH
)

target_sources(${TEST_NAME}
    PRIVATE
    ${mbed-os_SOURCE_DIR}/drivers/source/I2C.cpp
        test_i2c.cpp
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        mbed-headers-platform
        mbed-headers-hal
        mbed-headers-drivers
        mbed-stubs-platform
        mbed-stubs-hal
        mbed-stubs-drivers
)

gtest_discover_tests(${TEST_NAME} PROPERTIES LABELS "drivers")
