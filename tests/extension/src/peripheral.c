/**
 * @file      peripheral.c
 * @brief     Header of
 * @date      Mon Nov 29 16:54:38 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 * This module
 */


#include <logging/log.h>
#include "zbus.h"
LOG_MODULE_DECLARE(peripheral, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_SUBSCRIBER_REGISTER(peripheral, 16);


void peripheral_thread(void)
{
    struct sensor_data sd  = {0, 0};
    zbus_channel_index_t idx = 0;
    while (!k_msgq_get(peripheral.queue, &idx, K_FOREVER)) {
        sd.a += 1;
        sd.b += 10;
        LOG_DBG("[Peripheral] sending sensor data");
        zbus_chan_pub(sensor_data, sd, K_MSEC(250));
    }
}

K_THREAD_DEFINE(peripheral_thread_id, 1024, peripheral_thread, NULL, NULL, NULL, 3, 0, 0);
