cmake_minimum_required(VERSION 3.13)

add_library(common_definition INTERFACE)
target_include_directories(common_definition INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(common_definition INTERFACE pico_stdlib)
