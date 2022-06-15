target remote :3333
monitor start
file ./build/zephyr/zephyr.elf
load ./build/zephyr/zephyr.elf
b main
b proxy_callback
b 61
b mock_proxy_rx_thread
c

