cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH "C:/pico/pico-sdk")
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(pico_oscilloscope C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico_oscilloscope
    main.cpp
)

target_link_libraries(pico_oscilloscope
    pico_stdlib
    hardware_adc
    hardware_dma
)

pico_enable_stdio_usb(pico_oscilloscope 1)
pico_enable_stdio_uart(pico_oscilloscope 0)
pico_add_extra_outputs(pico_oscilloscope)