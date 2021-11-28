cmake_minimum_required(VERSION 3.13.1)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(zbus)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(app_sources
  # ../src/core.c
  ../src/main.c
  # ../src/net.c
  # ../src/peripheral.c
)
target_sources(app PRIVATE ${app_sources})
