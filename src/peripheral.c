#include <kernel.h>
#include <sys/printk.h>
#include "zbus.h"

void peripheral_thread(void)
{
    int a                 = 1;
    int b                 = 3;
    struct sensor_data sd = {0, 0};
    while (1) {
        k_msleep(50000000);
        sd.a = ++a;
        sd.b = ++b;
        zt_chan_pub(sensor_data, sd);
    }
}

K_THREAD_DEFINE(peripheral_thread_id, 1024, peripheral_thread, NULL, NULL, NULL, 3, 0, 0);
