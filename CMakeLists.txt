cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(zbus)


list(APPEND HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include")
list(
  APPEND SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/zbus.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/core.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/net.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/peripheral.c"
)

include_directories(${HEADERS})
target_sources(app PRIVATE ${SOURCES})
