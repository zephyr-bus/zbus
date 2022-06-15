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

ZBUS_SUBSCRIBER_DECLARE(core, 10);

void core_thread(void)
{
}

K_THREAD_DEFINE(core_thread_id, 1024, core_thread, NULL, NULL, NULL, 3, 0, 0);
