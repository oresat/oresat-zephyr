# Top-level CMakeLists.txt for the skeleton application.
#
# Copyright (c) 2017 Open Source Foundries Limited
# Copyright (c) 2018 Foundries.io Ltd
#
# SPDX-License-Identifier: Apache-2.0
#
# This provides a basic application structure suitable for loading by
# mcuboot, which is easy to customize on a per-board basis. It can be
# used as a starting point for new applications.

cmake_minimum_required(VERSION 3.8)

# Set CMake Variable to include dts overlay for flash partitions
set(DTC_OVERLAY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/boards/boot-stm32f091rc.overlay)

# find_package(Zephyr) in order to load application boilerplate:
# https://docs.zephyrproject.org/latest/develop/application/index.html
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(NONE)



