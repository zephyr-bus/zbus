/**
 * @file      net.c
 * @brief     Header of
 * @date      Sun Nov 28 18:03:22 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 * This module
 */

#include <kernel.h>
#include <sys/printk.h>
#include "zbus.h"

K_MSGQ_DEFINE(net_queue, sizeof(zt_channel_index_t), 10, 2);
/**
 * @brief Summary
 * @details Description
 */

void net_thread(void)
{
    zt_channel_index_t idx = 0;
    while (1) {
        if (!k_msgq_get(&net_queue, &idx, K_FOREVER)) {
            struct net_pkt pkt = {0};
            zt_chan_read(net_pkt, pkt);
            printk("Net: Parity %c, 3 multiple: %s\n", pkt.x, pkt.y ? "true" : "false");
        }
    }
}

K_THREAD_DEFINE(net_thread_id, 1024, net_thread, NULL, NULL, NULL, 3, 0, 0);
