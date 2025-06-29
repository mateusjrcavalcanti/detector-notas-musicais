# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(detector-notas-musicais C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

# Coleta dinamicamente todos os arquivos .c da pasta src
file(GLOB SRC_FILES "src/*.c")

add_executable(detector-notas-musicais main.c ${SRC_FILES})

pico_set_program_name(detector-notas-musicais "detector-notas-musicais")
pico_set_program_version(detector-notas-musicais "0.1")

# PIO
pico_generate_pio_header(detector-notas-musicais ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(detector-notas-musicais 0)
pico_enable_stdio_usb(detector-notas-musicais 1)

# Add the standard library to the build
target_link_libraries(detector-notas-musicais
        pico_stdlib
        hardware_adc
        hardware_pwm
        hardware_i2c
        hardware_pio
        )

# Add the standard include files to the build
target_include_directories(detector-notas-musicais PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
)

# Add any user requested libraries
target_link_libraries(detector-notas-musicais 
        
        )

pico_add_extra_outputs(detector-notas-musicais)

