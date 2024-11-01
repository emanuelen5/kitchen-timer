cmake_minimum_required(VERSION 3.5)
project(kitchen-timer)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/functions.cmake)
init_git_submodule(simavr)

set(SIMAVR_DIR "${CMAKE_SOURCE_DIR}/simavr")

add_executable(
    bootloader

    ${CMAKE_SOURCE_DIR}/src/bootloader.cpp

    ${SIMAVR_DIR}/simavr/sim/avr_acomp.c
    ${SIMAVR_DIR}/simavr/sim/avr_adc.c
    ${SIMAVR_DIR}/simavr/sim/avr_bitbang.c
    ${SIMAVR_DIR}/simavr/sim/avr_eeprom.c
    ${SIMAVR_DIR}/simavr/sim/avr_extint.c
    ${SIMAVR_DIR}/simavr/sim/avr_flash.c
    ${SIMAVR_DIR}/simavr/sim/avr_ioport.c
    ${SIMAVR_DIR}/simavr/sim/avr_lin.c
    ${SIMAVR_DIR}/simavr/sim/avr_spi.c
    ${SIMAVR_DIR}/simavr/sim/avr_timer.c
    ${SIMAVR_DIR}/simavr/sim/avr_twi.c
    ${SIMAVR_DIR}/simavr/sim/avr_uart.c
    ${SIMAVR_DIR}/simavr/sim/avr_usb.c
    ${SIMAVR_DIR}/simavr/sim/avr_usi.c
    ${SIMAVR_DIR}/simavr/sim/avr_watchdog.c
    # ${SIMAVR_DIR}/simavr/sim/run_avr.c  # We supply our own main function
    ${SIMAVR_DIR}/simavr/sim/sim_avr.c
    ${SIMAVR_DIR}/simavr/sim/sim_cmds.c
    ${SIMAVR_DIR}/simavr/sim/sim_core.c
    ${SIMAVR_DIR}/simavr/sim/sim_cycle_timers.c
    ${SIMAVR_DIR}/simavr/sim/sim_dwarf.c
    ${SIMAVR_DIR}/simavr/sim/sim_elf.c
    ${SIMAVR_DIR}/simavr/sim/sim_gdb.c
    ${SIMAVR_DIR}/simavr/sim/sim_hex.c
    ${SIMAVR_DIR}/simavr/sim/sim_interrupts.c
    ${SIMAVR_DIR}/simavr/sim/sim_io.c
    ${SIMAVR_DIR}/simavr/sim/sim_irq.c
    ${SIMAVR_DIR}/simavr/sim/sim_utils.c
    ${SIMAVR_DIR}/simavr/sim/sim_vcd_file.c

    ${SIMAVR_DIR}/simavr/cores/sim_megax8.c
    ${SIMAVR_DIR}/simavr/cores/sim_mega328.c
    ${SIMAVR_DIR}/examples/parts/uart_pty.c
)

target_include_directories(
    bootloader PUBLIC
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PUBLIC ${SIMAVR_DIR}/simavr
    PUBLIC ${SIMAVR_DIR}/simavr/sim
    PUBLIC ${SIMAVR_DIR}/simavr/cores
    PUBLIC ${SIMAVR_DIR}/examples/parts
)

set_property(TARGET bootloader PROPERTY C_STANDARD 99)

add_git_version_definition(bootloader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -Wextra -Wno-sign-compare")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -Wall -Wno-sign-compare")

option(WERROR "Turn warnings into errors" ON)
if(WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
endif()

