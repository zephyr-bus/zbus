# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zbus_as_a_module)

# this will help zephyr to find the zbus_messages ans zbus_channels files 
zephyr_include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

list(APPEND HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )
list(
  APPEND SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
  )

include_directories(${HEADERS})
target_sources(app PRIVATE ${SOURCES})
