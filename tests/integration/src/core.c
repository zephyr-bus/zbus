/**
 * @file      core.c
 * @brief     Header of
 * @date      Sun Nov 28 18:03:41 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 * This module
 */

#include <logging/log.h>
#include "zbus.h"
LOG_MODULE_REGISTER(core, CONFIG_ZBUS_LOG_LEVEL);

K_MSGQ_DEFINE(core_queue, sizeof(zb_channel_index_t), 10, sizeof(zb_channel_index_t));

void core_thread(void)
{
    zb_channel_index_t idx = 0;
    struct action start    = {true};
    zb_chan_pub(start_measurement, start, K_FOREVER);
    while (1) {
        if (!k_msgq_get(&core_queue, &idx, K_FOREVER)) {
            struct sensor_data data = {0};
            zb_chan_read(sensor_data, data, K_NO_WAIT);
            LOG_DBG("[Core] sensor {a = %d, b = %d}. Sending pkt", data.a, data.b);
            struct net_pkt pkt = {.x = !(data.a % 2) ? 'P' : 'I',
                                  .y = !(data.b % 3) ? true : false};
            zb_chan_pub(net_pkt, pkt, K_MSEC(200));
        }
    }
}

K_THREAD_DEFINE(core_thread_id, 1024, core_thread, NULL, NULL, NULL, 3, 0, 0);
