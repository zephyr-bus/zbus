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
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_SUBSCRIBER_DECLARE(net, 4);

struct net_pkt pkt = {0};

/**
 * @brief Summary
 * @details Description
 */

void net_thread(void)
{
    zbus_channel_index_t idx = 0;
    while (1) {
        if (!k_msgq_get(net.queue, &idx, K_FOREVER)) {
            ZBUS_CHAN_READ(net_pkt, pkt, K_NO_WAIT);
            LOG_DBG("[Net] Parity %c, 3 multiple: %s", pkt.x, pkt.y ? "true" : "false");
        }
    }
}

K_THREAD_DEFINE(net_thread_id, 1024, net_thread, NULL, NULL, NULL, 3, 0, 0);
