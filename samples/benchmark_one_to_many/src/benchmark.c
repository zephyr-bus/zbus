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

#define BYTES_TO_BE_SENT (256LLU * 1024LLU)
uint64_t count = 0;

#if (BM_ASYNC == 1)
ZBUS_SUBSCRIBER_REGISTER(s1, 4);
#if (BM_ONE_TO >= 2LLU)
ZBUS_SUBSCRIBER_REGISTER(s2, 4);
#if (BM_ONE_TO > 2LLU)
ZBUS_SUBSCRIBER_REGISTER(s3, 4);
ZBUS_SUBSCRIBER_REGISTER(s4, 4);
#if (BM_ONE_TO > 4LLU)
ZBUS_SUBSCRIBER_REGISTER(s5, 4);
ZBUS_SUBSCRIBER_REGISTER(s6, 4);
ZBUS_SUBSCRIBER_REGISTER(s7, 4);
ZBUS_SUBSCRIBER_REGISTER(s8, 4);
#if (BM_ONE_TO > 8LLU)
ZBUS_SUBSCRIBER_REGISTER(s9, 4);
ZBUS_SUBSCRIBER_REGISTER(s10, 4);
ZBUS_SUBSCRIBER_REGISTER(s11, 4);
ZBUS_SUBSCRIBER_REGISTER(s12, 4);
ZBUS_SUBSCRIBER_REGISTER(s13, 4);
ZBUS_SUBSCRIBER_REGISTER(s14, 4);
ZBUS_SUBSCRIBER_REGISTER(s15, 4);
ZBUS_SUBSCRIBER_REGISTER(s16, 4);
#endif
#endif
#endif
#endif

#define S_TASK(name)                                                                    \
    void name##_task()                                                                  \
    {                                                                                   \
        struct bm_msg msg_received = {0};                                               \
        zbus_channel_index_t idx   = ZBUS_CHANNEL_COUNT;                                \
        while (!k_msgq_get(name.queue, &idx, K_FOREVER)) {                              \
            if (!__zbus_chan_read(                                                      \
                    __zbus_metadata_get_by_id(idx), (uint8_t *) &msg_received,          \
                    __zbus_metadata_get_by_id(idx)->message_size, K_NO_WAIT)) {         \
                count += BM_MESSAGE_SIZE;                                               \
            }                                                                           \
        }                                                                               \
    }                                                                                   \
    K_THREAD_DEFINE(name##_id, BM_MESSAGE_SIZE + 192, name##_task, NULL, NULL, NULL, 3, \
                    0, 0);

S_TASK(s1)
#if (BM_ONE_TO >= 2LLU)
S_TASK(s2)
#if (BM_ONE_TO > 2LLU)
S_TASK(s3)
S_TASK(s4)
#if (BM_ONE_TO > 4LLU)
S_TASK(s5)
S_TASK(s6)
S_TASK(s7)
S_TASK(s8)
#if (BM_ONE_TO > 8LLU)
S_TASK(s9)
S_TASK(s10)
S_TASK(s11)
S_TASK(s12)
S_TASK(s13)
S_TASK(s14)
S_TASK(s15)
S_TASK(s16)
#endif
#endif
#endif
#endif

#else  // SYNC
void s_cb(zbus_channel_index_t idx);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s1, s_cb);
#if (BM_ONE_TO >= 2LLU)
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s2, s_cb);
#if (BM_ONE_TO > 2LLU)
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s3, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s4, s_cb);
#if (BM_ONE_TO > 4LLU)
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s5, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s6, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s7, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s8, s_cb);
#if (BM_ONE_TO > 8LLU)
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s9, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s10, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s11, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s12, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s13, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s14, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s15, s_cb);
ZBUS_SUBSCRIBER_REGISTER_CALLBACK(s16, s_cb);
#endif
#endif
#endif
#endif

zbus_message_variant_t msg_received = {0};
void s_cb(zbus_channel_index_t idx)
{
    if (!zbus_chan_read_by_index_unsafe(idx, msg_received, K_NO_WAIT)) {
        count += BM_MESSAGE_SIZE;
    }
}
#endif  // BM_ASYNC

void main(void)
{
    printk(" Benchmark 1 to %lld: %sSYNC transmission and message size %u\n", BM_ONE_TO,
           BM_ASYNC ? "A" : "", BM_MESSAGE_SIZE);
}

void producer_thread(void)
{
    struct bm_msg msg = {0};
    for (uint64_t i = (BM_MESSAGE_SIZE - 1); i > 0; --i) {
        msg.bytes[i] = i;
    }

    struct bm_msg msg_to_be_sent = {0};
    uint32_t start               = k_uptime_get_32();
    for (uint64_t internal_count = BYTES_TO_BE_SENT / BM_ONE_TO; internal_count > 0;
         internal_count -= BM_MESSAGE_SIZE) {
        memcpy(&msg_to_be_sent, &msg, BM_MESSAGE_SIZE);
        zbus_chan_pub(bm_channel, msg_to_be_sent, K_MSEC(100));
    }
    uint32_t duration = (k_uptime_get_32() - start);
    if (duration == 0) {
        printk("Duration error!\n");
        k_oops();
    }
    uint64_t i = (BYTES_TO_BE_SENT * 1000) / duration;
    uint64_t f = ((BYTES_TO_BE_SENT * 100000) / duration) % 100;
    printk(" - Bytes sent = %lld, received = %llu \n - Average data rate: "
           "%llu.%lluB/s\n - Duration: %ums\n",
           BYTES_TO_BE_SENT, count, i, f, duration);
    printk("\n@%u", duration);
}

K_THREAD_DEFINE(producer_thread_id, 1024, producer_thread, NULL, NULL, NULL, 5, 0, 5000);
