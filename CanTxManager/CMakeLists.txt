cmake_minimum_required(VERSION 3.13)

add_library(CanTxManager STATIC
    CanTxManagerTask.c
)

target_link_libraries(CanTxManager
    PRIVATE free_rtos
    PRIVATE hardware_driver
)

target_include_directories(CanTxManager PUBLIC include)
