test:
	${ZEPHYR_BASE}/scripts/twister -O ./build/twister-out -T tests -xZEPHYR_EXTRA_MODULES=../zbus
	${ZEPHYR_BASE}/scripts/twister -O ./build/twister-out -T samples -xZEPHYR_EXTRA_MODULES=../zbus

coverage:
	${ZEPHYR_BASE}/scripts/twister --coverage -p native_posix -O ./build/twister-out -T tests -xZEPHYR_EXTRA_MODULES=../zbus
