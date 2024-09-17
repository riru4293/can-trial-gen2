cmake_minimum_required(VERSION 3.13)

add_library(can_manager STATIC
    ${CMAKE_CURRENT_LIST_DIR}/can_manager.c
)

# Add the library
target_link_libraries(can_manager
    # My module
    PRIVATE hardware_driver
)

# Add the include directory
target_include_directories(can_manager
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)
