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
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

void peripheral_thread(void)
{
    struct sensor_msg sm = {0, 0};
    while (1) {
        sm.press += 1;
        sm.temp += 10;
        sm.humidity += 100;
        LOG_DBG("Sending sensor data...");
        zbus_chan_pub(sensor_data, sm, K_MSEC(250));
        k_msleep(1000); /* Fix this for the target. */
    }
}

K_THREAD_DEFINE(peripheral_thread_id, 1024, peripheral_thread, NULL, NULL, NULL, 5, 0, 0);
