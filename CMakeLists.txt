
if(CONFIG_ZBUS)
  zephyr_include_directories(
    ${ZEPHYR_ZBUS_MODULE_DIR}/include
  )

  zephyr_library()
  zephyr_library_sources(
    ${ZEPHYR_ZBUS_MODULE_DIR}/src/zbus.c
  )

 # zephyr_compile_definitions_ifdef(CONFIG_ZBUS_CANONICAL ZBUS_CANONICAL)
endif()
