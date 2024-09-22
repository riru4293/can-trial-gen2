cmake_minimum_required(VERSION 3.13)

add_library(common_include INTERFACE)
target_include_directories(common_include INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(common_include INTERFACE pico_stdlib)
