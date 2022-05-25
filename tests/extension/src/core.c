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
LOG_MODULE_DECLARE(core, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_SUBSCRIBER_REGISTER(core, 10);

void core_thread(void)
{
    struct action start = {false};
    LOG_DBG("[Core] sending start measurement with status %d", start.status);
    zbus_chan_pub(start_measurement, start, K_MSEC(500));
    k_msleep(1000);

    start.status = true;
    LOG_DBG("[Core] sending start measurement with status %d", start.status);
    zbus_chan_pub(start_measurement, start, K_MSEC(500));
    k_msleep(1000);

    start.status = false;
    LOG_DBG("[Core] sending start measurement with status %d", start.status);
    zbus_chan_pub(start_measurement, start, K_MSEC(500));
    k_msleep(1000);

    start.status = true;
    LOG_DBG("[Core] sending finish with %d", start.status);
    zbus_chan_pub(finish, start, K_MSEC(500));
}

K_THREAD_DEFINE(core_thread_id, 1024, core_thread, NULL, NULL, NULL, 3, 0, 0);
