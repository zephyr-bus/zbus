ARGS := -- -G'Unix Makefiles' -DCMAKE_EXPORT_COMPILE_COMMANDS=on -Wno-dev -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y -DZEPHYR_EXTRA_MODULES=`pwd`/../../../zbus
BOARD := hifive1_revb

all: run

clean:
	@echo " *** Clean project:"
	rm -rf build

build:
	@echo "\n *** Build project:"
	west build -b $(BOARD) $(ARGS)
	cp ./build/compile_commands.json .

rebuild: clean build

run_renode: rebuild
	@echo "\n *** Run project:"
	renode renode_setup.resc --console

run_posix: clean
	west build -b native_posix $(ARGS)
	./build/zephyr/zephyr.exe


tests_unit:
	$(ZEPHYR_BASE)/scripts/twister -O ./build/twister-out -T tests/unittests

menuconfig:
	west build -b hifive1_revb -t menuconfig  $(ARGS)
