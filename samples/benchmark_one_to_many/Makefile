# ARGS := -- -G'Unix Makefiles' -DCMAKE_EXPORT_COMPILE_COMMANDS=on -Wno-dev -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y
ARGS := -- -DCMAKE_EXPORT_COMPILE_COMMANDS=on 
ANALYZER := -DCMAKE_EXPORT_COMPILE_COMMANDS=on -DCONFIG_THREAD_ANALYZER=y -DCONFIG_THREAD_ANALYZER_USE_PRINTK=y -DCONFIG_THREAD_ANALYZER_AUTO=y -DCONFIG_THREAD_ANALYZER_AUTO_INTERVAL=5
# -Wno-dev # -DCONFIG_ZBUS_LOG_LEVEL_DBG=y -DCONFIG_COVERAGE=y
MSG_SIZE ?= 32 
ONE_TO ?= 1
ASYNC ?= 0
BENCHMARK := -DMSG_SIZE=$(MSG_SIZE) -DONE_TO=$(ONE_TO) -DASYNC=$(ASYNC)
ARGS_PRODUCTION := -- -DCONFIG_THREAD_ANALYZER=y -DCONFIG_THREAD_ANALYZER_USE_PRINTK=y -DCONFIG_THREAD_ANALYZER_AUTO=y -DCONFIG_THREAD_ANALYZER_AUTO_INTERVAL=5
BOARD ?= hifive1_revb

all: run

clean:
	@echo " *** Clean project:"
	rm -rf build

build:
	@echo "\n *** Build project:"
	west build -b $(BOARD) $(ARGS) $(BENCHMARK) -DZEPHYR_EXTRA_MODULES=`pwd`/../..
	cp ./build/compile_commands.json .

build_production:
	@echo "\n *** Build project for production:"
	west build -b $(BOARD) $(ARGS_PRODUCTION)

rebuild: clean build

benchmark: rebuild 
	west flash

serial_monitor:
	@python3 decoder.py

renode: rebuild
	@echo "\n *** Run project:"
	renode-test renode_profile.robot -r build/robot # --enable-xwt	

run_posix: clean
	west build -b native_posix -- -Wno-dev -DCONFIG_ZBUS_LOG_LEVEL_DBG=y
	./build/zephyr/zephyr.exe

menuconfig: rebuild
	west build -b $(BOARD) -t menuconfig  $(ARGS)

