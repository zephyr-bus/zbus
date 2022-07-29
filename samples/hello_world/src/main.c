/*
 * Copyright (c) 2022 Citrinio
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "kernel.h"
#include "zbus.h"
#include "zbus_messages.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

void listener_callback_example(zbus_chan_idx_t idx);
ZBUS_LISTENER_DECLARE(my_listener, listener_callback_example);

void listener_callback_example(zbus_chan_idx_t idx)
{
    struct acc_msg acc = {0};
    ZBUS_CHAN_READ_BY_INDEX(idx, acc, K_NO_WAIT);
    LOG_DBG("From listener -> Acc x=%d, y=%d, z=%d", acc.x, acc.y, acc.z);
}

ZBUS_SUBSCRIBER_DECLARE(my_subscriber, 4);
void subscriber_task()
{
    zbus_chan_idx_t idx;
    while (!k_msgq_get(my_subscriber.queue, &idx, K_FOREVER)) {
        struct acc_msg acc = {0};
        if (acc_data_chan_idx == idx) {
            ZBUS_CHAN_READ(acc_data, acc, K_NO_WAIT);
            LOG_DBG("From subscriber -> Acc x=%d, y=%d, z=%d", acc.x, acc.y, acc.z);
        }
    }
}
K_THREAD_DEFINE(subscriber_task_id, 512, subscriber_task, NULL, NULL, NULL, 3, 0, 0);


/* Do not use main thread to publish messages. It may generate an inconsistent behavior.
 * The default priority of main thread is the same as zbus event dispatcher. It can cause
 * a undesired zbus preemptions.
 *
 * To fix that you can change the main thread priority as we are doing here in this
 * example or use another thread with less priority. We suggest to use threads priority
 * from 3 and on.
 *
 * You always give the producer less priority than the consumer. It will guarantee the
 * expected behavior to happen.
 */
void main(void)
{
    /* The raw way to reading a channel's message */
    struct version v = {0};
    zbus_chan_read(ZBUS_CHAN_GET(version), (uint8_t*) &v, sizeof(v), K_NO_WAIT);
    LOG_DBG("Sensor sample started raw reading, version %u.%u-%u!", v.major, v.minor,
            v.build);

    /* The macro way to reading a channel's message */
    struct version v2 = {0};
    ZBUS_CHAN_READ(version, v2, K_NO_WAIT);
    LOG_DBG("Sensor sample started macro reading, version %u.%u-%u!", v2.major, v2.minor,
            v2.build);

    /* The raw way to publishing a channel's message */
    struct acc_msg acc1 = {.x = 1, .y = 1, .z = 1};
    zbus_chan_pub(ZBUS_CHAN_GET(acc_data), (uint8_t*) &acc1, sizeof(acc1), K_SECONDS(1));

    /* The macro way to publishing a channel's message */
    struct acc_msg acc2 = {.x = 2, .y = 2, .z = 2};
    ZBUS_CHAN_PUB(acc_data, acc2, K_MSEC(1000));
}
