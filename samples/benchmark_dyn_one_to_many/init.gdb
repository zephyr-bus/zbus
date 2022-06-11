target remote :3333
monitor start
file ./build/zephyr/zephyr.elf
load ./build/zephyr/zephyr.elf
b main
b zbus.c:zbus_dyn_chan_pub
b zbus.c:__zbus_monitor_thread
b benchmark.c:producer_thread
b benchmark.c:s_cb
c

