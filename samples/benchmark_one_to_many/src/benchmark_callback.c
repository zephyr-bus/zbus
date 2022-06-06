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

#define ONE_TO 16LLU
#include "zbus.h"
#include "zbus_messages.h"
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

/*! benchmark description
 * data variation from 1 to 256 bytes (1,2,4,8,16,32,64,128,256)
 * callback based first
 */
#define BYTES_TO_BE_SENT (256LL * 1024LLU)

void s_cb(zbus_channel_index_t idx);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s1, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s2, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s3, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s4, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s5, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s6, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s7, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s8, s_cb);
#if (ONE_TO == 16LLU)
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s9, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s10, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s11, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s12, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s13, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s14, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s15, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s16, s_cb);
#endif

// void fh1_cb(zbus_channel_index_t idx);
// ZBUS_SUBSCRIBER_REGISTER_CALLBACK(fast_handler1, fh1_cb);

zbus_channel_index_t current_idx = zbus_index_chan_8b;
uint16_t current_message_size    = 1;


uint64_t count                      = 0;
zbus_message_variant_t msg_received = {0};
void s_cb(zbus_channel_index_t idx)
{
    if (!zbus_chan_read_by_index_unsafe(idx, msg_received, K_NO_WAIT)) {
        count += current_message_size;
    }
}

void main(void)
{
    struct version v = {0};
    zbus_chan_read(version, v, K_NO_WAIT);

    LOG_DBG("Benchmark sample started, version %u.%u-%u!", v.major, v.minor, v.build);
}

void producer_thread(void)
{
    current_message_size = __zbus_metadata_get_by_id(current_idx)->message_size;

    uint8_t msg_data[256] = {0};
    for (uint64_t i = (current_message_size - 1); i > 0; --i) {
        msg_data[i] = i;
    }

    uint32_t start             = k_uptime_get_32();
    zbus_message_variant_t msg = {0};
    for (uint64_t internal_count = BYTES_TO_BE_SENT / ONE_TO; internal_count > 0;
         internal_count -= current_message_size) {
        memcpy(&msg, msg_data, current_message_size);
        zbus_chan_pub_by_index_unsafe(current_idx, msg, K_MSEC(250));
    }
    uint32_t duration = (k_uptime_get_32() - start);
    uint64_t i        = (BYTES_TO_BE_SENT * 1000) / duration;
    uint64_t f        = ((BYTES_TO_BE_SENT * 100000) / duration) % 100;
    printk("\n *** Bytes sent = %lld, received = %llu in %ums\n     Average data rate: "
           "%llu.%lluB/s\n\n\n",
           BYTES_TO_BE_SENT, count, duration, i, f);
}

K_THREAD_DEFINE(producer_thread_id, 2048, producer_thread, NULL, NULL, NULL, 5, 0, 10);
