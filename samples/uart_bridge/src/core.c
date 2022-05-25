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

K_MSGQ_DEFINE(core_queue, sizeof(zbus_channel_index_t), 10, sizeof(zbus_channel_index_t));

void core_thread(void)
{
    struct action start = {false};
    while (1) {
        LOG_DBG("[Core] sending start measurement with status %d", start.status);
        start.status = !start.status;
        zbus_chan_pub(start_measurement, start, K_MSEC(500));
        k_msleep(1000000);
    }
}

K_THREAD_DEFINE(core_thread_id, 1024, core_thread, NULL, NULL, NULL, 3, 0, 0);
