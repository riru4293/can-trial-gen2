cmake_minimum_required(VERSION 3.13)

# C standard features required for building
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
set(CMAKE_BUILD_TYPE Debug)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================

# Specify the target board (must be before pico_sdk_init)
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Define the project name
project(can_trial
    VERSION 2.0.0
    DESCRIPTION "Study CAN communication"
    LANGUAGES CXX C ASM
)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Include the sub modules
add_subdirectory(my_stdlib)         # Common definition
add_subdirectory(hardware_driver)   # Operate the hardware
add_subdirectory(free_rtos)         # FreeRTOS
add_subdirectory(can_manager)       # CAN communication manager

add_executable(can_trial ${CMAKE_CURRENT_LIST_DIR}/main.c)

# Add the standard library to the build
target_link_libraries(can_trial
    # FreeRTOS
    PRIVATE free_rtos
    # Driver
    PRIVATE hardware_driver
    # My module
    PRIVATE can_manager
)

pico_set_program_name(can_trial "can_trial")
pico_set_program_version(can_trial "1.0.0")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(can_trial 1)
pico_enable_stdio_usb(can_trial 0)

pico_add_extra_outputs(can_trial)
