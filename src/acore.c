/**
 * @file      core.c
 * @brief     Header of
 * @date      Sun Nov 28 18:03:41 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 * This module
 */

#include <kernel.h>
#include <sys/printk.h>
#include "zbus.h"

K_MSGQ_DEFINE(core_queue, sizeof(zt_channel_index_t), 10, 2);

void core_thread(void)
{
    zt_channel_index_t idx = 0;
    while (1) {
        if (!k_msgq_get(&core_queue, &idx, K_FOREVER)) {
            struct sensor_data data = {0};
            zt_chan_read(sensor_data, data);
            struct net_pkt pkt = {.x = data.a % 2 ? 'P' : 'I',
                                  .y = data.b % 3 ? true : false};
            zt_chan_pub(net_pkt, pkt);
        }
    }
}

K_THREAD_DEFINE(core_thread_id, 1024, core_thread, NULL, NULL, NULL, 3, 0, 0);
