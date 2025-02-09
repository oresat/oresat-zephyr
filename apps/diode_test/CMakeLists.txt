# SPDX-License-Identifier: Apache-2.0

include(CMakePrintHelpers)

cmake_minimum_required(VERSION 3.20.0)

macro(app_set_runner_args)
  board_runner_args(canopen "--node-id=${CONFIG_CANOPEN_NODE_ID}")
  board_runner_args(stm32cubeprogrammer 
    #"-V" 
    #"--download-address 0x0800C000" 
    "--port=swd" 
    "--reset-mode=hw"
    "-a 0x0800C000" 
  )
  #board_runner_args(stm32cubeprogrammer "--port=swd" "--reset-mode=hw")
endmacro()

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(CARD "diode_test")
project("oresat-${CARD}" LANGUAGES C)

execute_process(COMMAND bash -c "oresat-configs fw-files ${CARD} -d gen")

target_compile_options(app 
  PRIVATE 
    -Wall 
    -Wextra 
    #  -Wundef 
    -Wstrict-prototypes
)

zephyr_include_directories(gen)

target_include_directories(app 
  PRIVATE 
    src/
    #    ${COMMON_SRC}/include/
)

target_sources(app 
  PRIVATE 
    src/main.c
    gen/OD.c
    src/diode_test.c
    #    ${COMMON_SRC}/oresat.c
)

