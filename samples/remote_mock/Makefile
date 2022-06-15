# ARGS := -- -G'Unix Makefiles' -DCMAKE_EXPORT_COMPILE_COMMANDS=on -Wno-dev -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y
ARGS := -- -DCMAKE_EXPORT_COMPILE_COMMANDS=on -Wno-dev # -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y
ARGS_PRODUCTION := -- -Wno-dev
BOARD ?= hifive1_revb

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

serial_monitor:
	@python3 remote_mock.py

run: rebuild
	@echo "\n *** Run project:"
	renode renode_setup.resc --console

renode_debug: rebuild
	@echo "\n *** Debug project in Renode"
	renode renode_debug.resc --console 

run_posix: clean
	west build -b native_posix -- -Wno-dev -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y
	./build/zephyr/zephyr.exe

menuconfig: rebuild
	west build -b $(BOARD) -t menuconfig  $(ARGS)
