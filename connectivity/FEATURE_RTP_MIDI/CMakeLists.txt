# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests/UNITTESTS)
endif()

target_include_directories(mbed-rtp-midi
    INTERFACE
        include
)

target_sources(mbed-rtp-midi
    INTERFACE
)

target_link_libraries(mbed-rtp-midi
    INTERFACE
)


