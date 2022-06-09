# ARGS := -- -G'Unix Makefiles' -DCMAKE_EXPORT_COMPILE_COMMANDS=on -Wno-dev -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y
ARGS := -- -DCMAKE_EXPORT_COMPILE_COMMANDS=on 
ANALYZER := -DCMAKE_EXPORT_COMPILE_COMMANDS=on -DCONFIG_THREAD_ANALYZER=y -DCONFIG_THREAD_ANALYZER_USE_PRINTK=y -DCONFIG_THREAD_ANALYZER_AUTO=y -DCONFIG_THREAD_ANALYZER_AUTO_INTERVAL=5
# -Wno-dev # -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y
ARGS_PRODUCTION := -- -DCONFIG_THREAD_ANALYZER=y -DCONFIG_THREAD_ANALYZER_USE_PRINTK=y -DCONFIG_THREAD_ANALYZER_AUTO=y -DCONFIG_THREAD_ANALYZER_AUTO_INTERVAL=5
BOARD ?= hifive1_revb

all: run

clean:
	@echo " *** Clean project:"
	rm -rf build

build:
	@echo "\n *** Build project:"
	west build -b $(BOARD) $(ARGS) -DZEPHYR_EXTRA_MODULES=`pwd`/../..
	cp ./build/compile_commands.json .

rebuild: clean build

benchmark: rebuild 
	west flash

serial_monitor:
	@python3 decoder.py

run: rebuild
	@echo "\n *** Run project:"
	renode renode_setup.resc --console

run_posix: clean
	west build -b native_posix -- -Wno-dev -DCONFIG_ZBUS_LOG_LEVEL_DBG=y
	./build/zephyr/zephyr.exe

menuconfig: rebuild
	west build -b $(BOARD) -t menuconfig  $(ARGS)

