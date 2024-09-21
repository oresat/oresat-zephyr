# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(ORESAT_FIRMWARE_ROOT "$ENV{ZEPHYR_BASE}/../..")

list(APPEND BOARD_ROOT ${ORESAT_FIRMWARE_ROOT}/)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_protocard)

target_compile_options(app 
  PRIVATE 
    -Wall 
    -Wextra 
    #  -Wundef 
    -Wstrict-prototypes
)

target_sources(app 
  PRIVATE 
    main.c
    src/protocard.c
)
target_include_directories(app PRIVATE include/)
