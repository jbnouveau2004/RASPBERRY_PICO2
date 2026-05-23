cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_http_server C CXX ASM)

pico_sdk_init()

add_executable(server
    main.c
)

target_link_libraries(server
    pico_stdlib
    pico_cyw43_arch_lwip_poll
    pico_lwip_mbedtls
    pico_mbedtls
)

pico_enable_stdio_usb(server 1)
pico_enable_stdio_uart(server 0)

pico_add_extra_outputs(server)

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)