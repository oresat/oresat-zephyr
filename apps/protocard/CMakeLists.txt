# SPDX-License-Identifier: Apache-2.0

include(CMakePrintHelpers)

cmake_minimum_required(VERSION 3.20.0)

set(ORESAT_ROOT "$ENV{ZEPHYR_BASE}/../..")
set(COMMON_SRC "${ORESAT_ROOT}/common")
#cmake_print_variables(ORESAT_ROOT)
#cmake_print_variables(COMMON_SRC)

list(APPEND BOARD_ROOT ${ORESAT_ROOT}/)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(CARD "diode_test")
project("oresat-${CARD}" LANGUAGES C)

execute_process(COMMAND bash -c "oresat-configs fw-files ${CARD} -d gen")

target_sources(app 
  PRIVATE 
    main.c
    gen/OD.c
    src/diode_test.c
    #    src/blinky.c
    #    ${COMMON_SRC}/oresat.c
)

zephyr_include_directories(gen)

target_compile_options(app 
  PRIVATE 
    -Wall 
    -Wextra 
    #  -Wundef 
    -Wstrict-prototypes
)

target_include_directories(app 
  PRIVATE 
    src/include/
    ${COMMON_SRC}/include/
    ${COMMON_SRC}/canopennode_v4/
)
