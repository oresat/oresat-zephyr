cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(CARD "rw")
project("oresat-${CARD}" LANGUAGES C)

execute_process(COMMAND bash -c "oresat-configs fw-files ${CARD} -d gen")

target_sources(app PRIVATE
  src/main.c
  gen/OD.c
)

zephyr_include_directories(gen)
