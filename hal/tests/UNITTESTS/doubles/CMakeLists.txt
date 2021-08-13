# Copyright (c) 2021 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(mbed-headers-hal INTERFACE)

target_include_directories(mbed-headers-hal
    INTERFACE
        .
        ${mbed-os_SOURCE_DIR}/hal
        ${mbed-os_SOURCE_DIR}/hal/include
)

add_library(mbed-stubs-hal)

target_compile_definitions(mbed-stubs-hal
    PRIVATE
        DEVICE_PWMOUT
        DEVICE_WATCHDOG
        MBED_WDOG_ASSERT=1
        DEVICE_ANALOGIN
        DEVICE_ANALOGOUT
        DEVICE_FLASH
        DEVICE_I2C
        DEVICE_I2C_ASYNCH
        DEVICE_I2CSLAVE
        DEVICE_CAN
        DEVICE_INTERRUPTIN
        DEVICE_CRC
        DEVICE_OSPI
        DEVICE_PORTIN
        DEVICE_PORTOUT
        DEVICE_QSPI
        DEVICE_SPI
        DEVICE_SPI_COUNT
        DEVICE_SPI_ASYNCH
        DEVICE_SERIAL
        DEVICE_SERIAL_ASYNCH
        DEVICE_SERIAL_FC
        DEVICE_ITM
        DEVICE_LPTICKER
)

target_sources(mbed-stubs-hal
    PRIVATE
        pwmout_api_stub.c
        us_ticker_stub.cpp
        watchdog_api_stub.c
        analogin_api_stub.c
        analogout_api_stub.c
        gpio_api_stub.c
        flash_api_stub.c
        i2c_api_stub.c
        can_api_stub.c
        gpio_irq_api_stub.c
        ospi_api_stub.c
        port_api_stub.c
        qspi_api_stub.c
        spi_api_stub.c
        serial_api_stub.c
        itm_api_stub.c
        ticker_api_stub.c
        lp_ticker_api_stub.c
)

target_link_libraries(mbed-stubs-hal
    PRIVATE
        mbed-headers-hal
        mbed-headers-platform
        mbed-stubs-headers
)
