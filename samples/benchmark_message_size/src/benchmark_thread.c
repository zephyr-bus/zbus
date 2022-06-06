/*
 * Copyright (c) 2022 Citrinio
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

/*! benchmark description
 * data variation from 1 to 256 bytes (1,2,4,8,16,32,64,128,256)
 * callback based first
 */
#define BYTES_TO_BE_SENT (256LL * 1024)
void fh1_cb(zbus_channel_index_t idx);


ZBUS_SUBSCRIBER_REGISTER(fast_handler1, 16);


zbus_channel_index_t current_idx = zbus_index_chan_1b;
size_t current_message_size      = 0;

uint64_t count                      = 0;
zbus_message_variant_t msg_received = {0};
void thread_handler1_task()
{
    zbus_channel_index_t idx = ZBUS_CHANNEL_COUNT;
    while (!k_msgq_get(fast_handler1.queue, &idx, K_FOREVER)) {
        if (!zbus_chan_read_by_index_unsafe(idx, msg_received, K_NO_WAIT)) {
            count += current_message_size;
        }
    }
}

K_THREAD_DEFINE(thread_handler1_id, 1024, thread_handler1_task, NULL, NULL, NULL, 3, 0,
                0);

void main(void)
{
    struct version v = {0};
    zbus_chan_read(version, v, K_NO_WAIT);

    LOG_DBG("Benchmark sample started, version %u.%u-%u!", v.major, v.minor, v.build);
}

void producer_thread(void)
{
    zbus_message_variant_t msg_sent = {0};
    for (uint8_t i = 255; i > 0; --i) {
        msg_sent.chan_256b.bytes[i] = i;
    }
    current_message_size = __zbus_metadata_get_by_id(current_idx)->message_size;
    uint32_t start       = k_uptime_get_32();
    for (uint64_t internal_count = BYTES_TO_BE_SENT; internal_count > 0;
         internal_count -= current_message_size) {
        zbus_chan_pub_by_index_unsafe(current_idx, msg_sent, K_MSEC(250));
    }
    uint32_t duration = (k_uptime_get_32() - start);
    uint64_t i        = (BYTES_TO_BE_SENT * 1000) / duration;
    uint64_t f        = ((BYTES_TO_BE_SENT * 100000) / duration) % 100;
    printk("\n *** Bytes sent = %lld, received = %llu in %ums\n     Average data rate: "
           "%llu.%lluB/s\n\n\n",
           BYTES_TO_BE_SENT, count, duration, i, f);
}

K_THREAD_DEFINE(producer_thread_id, 1024, producer_thread, NULL, NULL, NULL, 5, 0, 10);
