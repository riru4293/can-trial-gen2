cmake_minimum_required(VERSION 3.13)

add_library(network_manager STATIC
    ${CMAKE_CURRENT_LIST_DIR}/ntm_task.c
)

# Add the library
target_link_libraries(network_manager
    # FreeRTOS
    PRIVATE free_rtos
    # My module
    PRIVATE hardware_driver
)

# Add the include directory
target_include_directories(network_manager
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
