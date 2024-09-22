cmake_minimum_required(VERSION 3.13)

add_library(hardware_driver STATIC
    ${CMAKE_CURRENT_LIST_DIR}/hardware_driver.c
    ${CMAKE_CURRENT_LIST_DIR}/rp2040.c
    ${CMAKE_CURRENT_LIST_DIR}/mcp2515.c
    ${CMAKE_CURRENT_LIST_DIR}/mcp2515_recv.c
    ${CMAKE_CURRENT_LIST_DIR}/mcp2515_send.c
)

# Add the library
target_link_libraries(hardware_driver
    # My standard library
    PUBLIC my_stdlib
    # RP2040
    PRIVATE hardware_spi
)

# Add the include directory
target_include_directories(hardware_driver
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
