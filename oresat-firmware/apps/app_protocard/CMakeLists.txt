# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
list(APPEND BOARD_ROOT $ENV{ZEPHYR_BASE}/../oresat-firmware/)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_protocard)

target_compile_options(
  app PRIVATE 
  -Wall 
  -Wextra 
  #  -Wundef 
  -Wstrict-prototypes
)

target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/blinky.c)
target_include_directories(app PRIVATE src/include/)
#target_include_directories(test PRIVATE ${YOUR_DIRECTORY})
#target_sources(app PRIVATE src/protocard.c)
