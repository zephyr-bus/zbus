/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "kernel.h"
#include "sys/util_macro.h"
#include "zbus.h"

#include <logging/log.h>
#include "zbus.h"
#include "zbus_messages.h"
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

void fh1_cb(zbus_chan_idx_t idx);
ZBUS_LISTENER_DECLARE(fast_handler1, fh1_cb);

void fh2_cb(zbus_chan_idx_t idx);
ZBUS_LISTENER_DECLARE(fast_handler2, fh2_cb);

void fh3_cb(zbus_chan_idx_t idx);
ZBUS_LISTENER_DECLARE(fast_handler3, fh3_cb);

void dh1_cb(zbus_chan_idx_t idx);
ZBUS_LISTENER_DECLARE(delay_handler1, dh1_cb);

void dh2_cb(zbus_chan_idx_t idx);
ZBUS_LISTENER_DECLARE(delay_handler2, dh2_cb);

void dh3_cb(zbus_chan_idx_t idx);
ZBUS_LISTENER_DECLARE(delay_handler3, dh3_cb);

struct sensor_msg msg = {0};
void fh1_cb(zbus_chan_idx_t idx)
{
    ZBUS_CHAN_READ_BY_INDEX(idx, msg, K_NO_WAIT);
    printk("Sensor msg processed by CALLBACK fh1: temp = %u, press = %u, humidity = %u\n",
           msg.temp, msg.press, msg.humidity);
}
void fh2_cb(zbus_chan_idx_t idx)
{
    ZBUS_CHAN_READ_BY_INDEX(idx, msg, K_NO_WAIT);
    printk("Sensor msg processed by CALLBACK fh2: temp = %u, press = %u, humidity = %u\n",
           msg.temp, msg.press, msg.humidity);
}
void fh3_cb(zbus_chan_idx_t idx)
{
    ZBUS_CHAN_READ_BY_INDEX(idx, msg, K_NO_WAIT);
    printk("Sensor msg processed by CALLBACK fh3: temp = %u, press = %u, humidity = %u\n",
           msg.temp, msg.press, msg.humidity);
}


struct sensor_wq_info {
    struct k_work work;
    zbus_chan_idx_t idx;
    uint8_t handle;
};

struct sensor_wq_info wq_handler1 = {.handle = 1};
struct sensor_wq_info wq_handler2 = {.handle = 2};
struct sensor_wq_info wq_handler3 = {.handle = 3};

void wq_dh_cb(struct k_work *item)
{
    struct sensor_wq_info *sens = CONTAINER_OF(item, struct sensor_wq_info, work);
    ZBUS_CHAN_READ_BY_INDEX(sens->idx, msg, K_NO_WAIT);
    printk("Sensor msg processed by WORK QUEUE handler dh%u: temp = %u, press = %u, "
           "humidity = %u\n",
           sens->handle, msg.temp, msg.press, msg.humidity);
}

void dh1_cb(zbus_chan_idx_t idx)
{
    wq_handler1.idx = idx;
    k_work_submit(&wq_handler1.work);
}

void dh2_cb(zbus_chan_idx_t idx)
{
    wq_handler2.idx = idx;
    k_work_submit(&wq_handler2.work);
}

void dh3_cb(zbus_chan_idx_t idx)
{
    wq_handler3.idx = idx;
    k_work_submit(&wq_handler3.work);
}

void main(void)
{
    k_work_init(&wq_handler1.work, wq_dh_cb);
    k_work_init(&wq_handler2.work, wq_dh_cb);
    k_work_init(&wq_handler3.work, wq_dh_cb);

    struct version v = {0};
    ZBUS_CHAN_READ(version, v, K_NO_WAIT);

    LOG_DBG("Sensor sample started, version %u.%u-%u!", v.major, v.minor, v.build);
}

ZBUS_SUBSCRIBER_DECLARE(thread_handler1, 4);
void thread_handler1_task()
{
    zbus_chan_idx_t idx = ZBUS_CHANNEL_COUNT;
    while (!k_msgq_get(thread_handler1.queue, &idx, K_FOREVER)) {
        ZBUS_CHAN_READ_BY_INDEX(idx, msg, K_NO_WAIT);
        printk("Sensor msg processed by THREAD handler: temp = %u, press = %u, "
               "humidity = %u\n",
               msg.temp, msg.press, msg.humidity);
    }
}

K_THREAD_DEFINE(thread_handler1_id, 1024, thread_handler1_task, NULL, NULL, NULL, 3, 0,
                0);

ZBUS_SUBSCRIBER_DECLARE(thread_handler2, 4);
void thread_handler2_task()
{
    zbus_chan_idx_t idx = ZBUS_CHANNEL_COUNT;
    while (!k_msgq_get(thread_handler2.queue, &idx, K_FOREVER)) {
        ZBUS_CHAN_READ_BY_INDEX(idx, msg, K_NO_WAIT);
        printk("Sensor msg processed by THREAD handler: temp = %u, press = %u, "
               "humidity = %u\n",
               msg.temp, msg.press, msg.humidity);
    }
}

K_THREAD_DEFINE(thread_handler2_id, 1024, thread_handler2_task, NULL, NULL, NULL, 3, 0,
                0);

ZBUS_SUBSCRIBER_DECLARE(thread_handler3, 4);
void thread_handler3_task()
{
    zbus_chan_idx_t idx = ZBUS_CHANNEL_COUNT;
    while (!k_msgq_get(thread_handler3.queue, &idx, K_FOREVER)) {
        ZBUS_CHAN_READ_BY_INDEX(idx, msg, K_NO_WAIT);
        printk("Sensor msg processed by THREAD handler: temp = %u, press = %u, "
               "humidity = %u\n",
               msg.temp, msg.press, msg.humidity);
    }
}

K_THREAD_DEFINE(thread_handler3_id, 1024, thread_handler3_task, NULL, NULL, NULL, 3, 0,
                0);
