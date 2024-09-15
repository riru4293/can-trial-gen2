cmake_minimum_required(VERSION 3.13)

# Set directory variables
set(FREERTOS_CFG_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Config")
set(FREERTOS_SRC_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel")

# Add FreeRTOS as a library
add_library(free_rtos STATIC
    ${FREERTOS_SRC_DIRECTORY}/event_groups.c
    ${FREERTOS_SRC_DIRECTORY}/list.c
    ${FREERTOS_SRC_DIRECTORY}/queue.c
    ${FREERTOS_SRC_DIRECTORY}/stream_buffer.c
    ${FREERTOS_SRC_DIRECTORY}/tasks.c
    ${FREERTOS_SRC_DIRECTORY}/timers.c
    ${FREERTOS_SRC_DIRECTORY}/portable/MemMang/heap_3.c
    ${FREERTOS_SRC_DIRECTORY}/portable/GCC/ARM_CM0/port.c
)

# Build FreeRTOS
target_include_directories(free_rtos
    PUBLIC
        ${FREERTOS_CFG_DIRECTORY}/
        ${FREERTOS_SRC_DIRECTORY}/include
        ${FREERTOS_SRC_DIRECTORY}/portable/GCC/ARM_CM0
)
