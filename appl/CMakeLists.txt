cmake_minimum_required(VERSION 3.13)

# Include modules.
#add_subdirectory(hardware_driver)

add_executable(can_trial ${CMAKE_CURRENT_LIST_DIR}/main.c)

# Add the standard library to the build
target_link_libraries(can_trial
PRIVATE pico_stdlib
  # FreeRTOS
  PRIVATE free_rtos
  # Driver
  PRIVATE hardware_driver
)

pico_set_program_name(can_trial "can_trial")
pico_set_program_version(can_trial "1.0.0")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(can_trial 1)
pico_enable_stdio_usb(can_trial 0)

pico_add_extra_outputs(can_trial)
