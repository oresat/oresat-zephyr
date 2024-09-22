# SPDX-License-Identifier: Apache-2.0

include(CMakePrintHelpers)

cmake_minimum_required(VERSION 3.20.0)

set(ORESAT_ROOT "$ENV{ZEPHYR_BASE}/../..")
set(COMMON_SRC "${ORESAT_ROOT}/common")
cmake_print_variables(ORESAT_ROOT)
cmake_print_variables(COMMON_SRC)

list(APPEND BOARD_ROOT ${ORESAT_ROOT}/)

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
    src/blinky.c
    #    ${COMMON_SRC}/oresat.c
)

target_sources(app 
  PRIVATE 
    ${COMMON_SRC}/oresat.c
    ${COMMON_SRC}/worker.c
)

target_include_directories(app 
  PRIVATE 
    src/include/
    ${COMMON_SRC}/include/
)
