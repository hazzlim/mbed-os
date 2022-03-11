# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

set(TEST_NAME spi-unittest)

add_executable(${TEST_NAME})

target_compile_definitions(${TEST_NAME}
    PRIVATE
        DEVICE_SPI
        DEVICE_SPI_COUNT
        DEVICE_SPI_ASYNCH
        TRANSACTION_QUEUE_SIZE_SPI
)

target_sources(${TEST_NAME}
    PRIVATE
    ${mbed-os_SOURCE_DIR}/drivers/source/SPI.cpp
        test_spi.cpp
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        mbed-headers-platform
        mbed-headers-hal
        mbed-headers-drivers
        mbed-stubs-platform
        mbed-stubs-hal
)

gtest_discover_tests(${TEST_NAME} PROPERTIES LABELS "drivers")
