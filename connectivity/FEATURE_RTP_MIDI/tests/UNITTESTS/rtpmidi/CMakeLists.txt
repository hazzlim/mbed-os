# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

set(TEST_NAME rtp-midi)

add_executable(${TEST_NAME})

target_sources(${TEST_NAME}
    PRIVATE
        test_rtpmidi.cpp
)

target_link_libraries(${TEST_NAME}
    PRIVATE
        gmock_main
)

gtest_discover_tests(${TEST_NAME} PROPERTIES LABELS "rtpmidi")
