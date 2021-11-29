#include <logging/log.h>
#include "zbus.h"
LOG_MODULE_REGISTER(peripheral, CONFIG_ZBUS_LOG_LEVEL);

K_MSGQ_DEFINE(peripheral_queue, sizeof(zt_channel_index_t), 10, 2);


void peripheral_thread(void)
{
    int a                 = 1;
    int b                 = 3;
    struct sensor_data sd = {0, 0};


    zt_channel_index_t idx = 0;
    if (!k_msgq_get(&peripheral_queue, &idx, K_FOREVER)) {
        LOG_DBG("[Peripheral] starting measurement");
    }
    while (1) {
        k_msleep(5000000);
        ++a;
        sd.a = a;
        ++b;
        sd.b = b;
        LOG_DBG("[Peripheral] sending sensor data");
        zt_chan_pub(sensor_data, sd);
        zt_chan_pub(sensor_data, sd);
    }
}

K_THREAD_DEFINE(peripheral_thread_id, 1024, peripheral_thread, NULL, NULL, NULL, 3, 0, 0);

void peripheral_digest_thread(void)
{
    zt_channel_index_t idx = 0;
}
