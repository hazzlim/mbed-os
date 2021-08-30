# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

set(TEST_NAME rtp-midi)

add_executable(${TEST_NAME})

target_sources(${TEST_NAME}
    PRIVATE
        ${mbed-os_SOURCE_DIR}/connectivity/netsocket/source/InternetSocket.cpp
        ${mbed-os_SOURCE_DIR}/connectivity/netsocket/source/InternetDatagramSocket.cpp
        ${mbed-os_SOURCE_DIR}/connectivity/netsocket/source/UDPSocket.cpp
        test_rtpmidi.cpp
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        mbed-headers-drivers
        mbed-headers-hal
        mbed-headers-rtos
        mbed-headers-platform
        mbed-headers-netsocket
        mbed-stubs-netsocket
        mbed-stubs-events
        mbed-stubs-platform
        mbed-stubs-rtos
        mbed-rtp-midi
        gmock_main
)

gtest_discover_tests(${TEST_NAME} PROPERTIES LABELS "rtpmidi")
