# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

set(TEST_NAME digitalout-unittest)

add_executable(${TEST_NAME})

target_compile_definitions(${TEST_NAME}
    PRIVATE
)

target_sources(${TEST_NAME}
    PRIVATE
        ${mbed-os_SOURCE_DIR}/drivers/source/DigitalOut.cpp
        test_digitalout.cpp
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        mbed-headers-platform
        mbed-headers-hal
        mbed-headers-drivers
        mbed-stubs-hal
        mbed-stubs-platform
)

gtest_discover_tests(${TEST_NAME} PROPERTIES LABELS "drivers")
