cmake_minimum_required(VERSION 3.13)
 
# initialize the SDK based on PICO_SDK_PATH 
# note: this must happen before project() 
include(pico_sdk_import.cmake)
 
project(pico)
 
# initialize the Raspberry Pi Pico SDK 
pico_sdk_init() 
 
# rest of your project 
add_executable(pwm 
    pwm_test.c
) 
 
# Add pico_stdlib library which aggregates commonly used features 
target_link_libraries(
    pwm
    pico_stdlib
    pico_multicore
    hardware_pwm
    hardware_dma
    hardware_pio
    hardware_timer
    hardware_clocks
) 
 
# create map/bin/hex/uf2 file in addition to ELF. 
pico_add_extra_outputs(pwm)