ARGS := -- -G'Unix Makefiles' -DCMAKE_EXPORT_COMPILE_COMMANDS=on -Wno-dev -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y
ARGS_PRODUCTION := -- -G'Unix Makefiles' -Wno-dev
BOARD := hifive1_revb

all: run

clean:
	@echo " *** Clean project:"
	rm -rf build

build:
	@echo "\n *** Build project:"
	west build -b $(BOARD) $(ARGS)
	cp ./build/compile_commands.json .

build_production:
	@echo "\n *** Build project for production:"
	west build -b $(BOARD) $(ARGS_PRODUCTION)

rebuild: clean build

run: rebuild
	@echo "\n *** Run project:"
	renode renode_setup.resc --console

run_posix: clean
	west build -b native_posix -- -Wno-dev -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y
	./build/zephyr/zephyr.exe


tests_unit:
	$(ZEPHYR_BASE)/scripts/twister -O ./build/twister-out -T tests/unittests

menuconfig:
	west build -b hifive1_revb -t menuconfig  $(ARGS)
