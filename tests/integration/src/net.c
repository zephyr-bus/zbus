/**
 * @file      net.c
 * @brief     Header of
 * @date      Sun Nov 28 18:03:22 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 * This module
 */

#include <logging/log.h>
#include "zbus.h"
#include "zbus_messages.h"
LOG_MODULE_REGISTER(net, CONFIG_ZBUS_LOG_LEVEL);

K_MSGQ_DEFINE(net_queue, sizeof(zb_channel_index_t), 10, 2);

struct net_pkt pkt = {0};

/**
 * @brief Summary
 * @details Description
 */

void net_thread(void)
{
    zb_channel_index_t idx = 0;
    while (1) {
        if (!k_msgq_get(&net_queue, &idx, K_FOREVER)) {
            zb_chan_read(net_pkt, pkt);
            LOG_DBG("[Net] Parity %c, 3 multiple: %s", pkt.x, pkt.y ? "true" : "false");
        }
    }
}

K_THREAD_DEFINE(net_thread_id, 1024, net_thread, NULL, NULL, NULL, 3, 0, 0);
