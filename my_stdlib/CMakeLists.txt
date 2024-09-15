cmake_minimum_required(VERSION 3.13)

add_library(my_stdlib INTERFACE)
target_include_directories(my_stdlib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(my_stdlib INTERFACE pico_stdlib)
