/**
 * @file      core.c
 * @brief     Header of
 * @date      Sun Nov 28 18:03:41 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 * This module
 */

#include "zbus.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_SUBSCRIBER_DECLARE(core, 8);

void core_thread(void)
{
    zbus_channel_index_t idx = 0;
    struct action start    = {true};
    ZBUS_CHAN_PUB(start_measurement, start, K_FOREVER);
    while (1) {
        if (!k_msgq_get(core.queue, &idx, K_FOREVER)) {
            struct sensor_data data = {0};
            ZBUS_CHAN_READ(sensor_data, data, K_NO_WAIT);
            LOG_DBG("[Core] sensor {a = %d, b = %d}. Sending pkt", data.a, data.b);
            struct net_pkt pkt = {.x = !(data.a % 2) ? 'P' : 'I',
                                  .y = !(data.b % 3) ? true : false};
            ZBUS_CHAN_PUB(net_pkt, pkt, K_MSEC(200));
        }
    }
}

K_THREAD_DEFINE(core_thread_id, 1024, core_thread, NULL, NULL, NULL, 3, 0, 0);
