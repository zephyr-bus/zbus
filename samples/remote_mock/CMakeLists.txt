# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(integration)

list(APPEND HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )
list(
  APPEND SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/../../src/zbus.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/mock_proxy.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ct_uart.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/peripheral.c"
  )

include_directories(${HEADERS})
target_sources(app PRIVATE ${SOURCES})
