/*
 * Copyright (c) 2022 Citrinio
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "fatal.h"
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
ZBUS_SUBSCRIBER_DECLARE(s1, 4);
#if (BM_ONE_TO >= 2LLU)
ZBUS_SUBSCRIBER_DECLARE(s2, 4);
#if (BM_ONE_TO > 2LLU)
ZBUS_SUBSCRIBER_DECLARE(s3, 4);
ZBUS_SUBSCRIBER_DECLARE(s4, 4);
#if (BM_ONE_TO > 4LLU)
ZBUS_SUBSCRIBER_DECLARE(s5, 4);
ZBUS_SUBSCRIBER_DECLARE(s6, 4);
ZBUS_SUBSCRIBER_DECLARE(s7, 4);
ZBUS_SUBSCRIBER_DECLARE(s8, 4);
#if (BM_ONE_TO > 8LLU)
ZBUS_SUBSCRIBER_DECLARE(s9, 4);
ZBUS_SUBSCRIBER_DECLARE(s10, 4);
ZBUS_SUBSCRIBER_DECLARE(s11, 4);
ZBUS_SUBSCRIBER_DECLARE(s12, 4);
ZBUS_SUBSCRIBER_DECLARE(s13, 4);
ZBUS_SUBSCRIBER_DECLARE(s14, 4);
ZBUS_SUBSCRIBER_DECLARE(s15, 4);
ZBUS_SUBSCRIBER_DECLARE(s16, 4);
#endif
#endif
#endif
#endif

#define S_TASK(name)                                                                    \
    void name##_task()                                                                  \
    {                                                                                   \
        struct bm_msg msg_received                    = {0};                            \
        struct external_data_msg *actual_message_data = NULL;                           \
        zbus_chan_idx_t idx                           = ZBUS_CHAN_COUNT;                \
        while (!k_msgq_get(name.queue, &idx, K_FOREVER)) {                              \
            zbus_chan_claim(ZBUS_CHAN_GET(bm_channel), K_NO_WAIT);                      \
            actual_message_data =                                                       \
                (struct external_data_msg *) ZBUS_CHAN_GET(bm_channel)->message;        \
            ZBUS_ASSERT(actual_message_data->reference != NULL);                        \
            memcpy(&msg_received, actual_message_data->reference,                       \
                   sizeof(struct bm_msg));                                              \
            zbus_chan_finish(ZBUS_CHAN_GET(bm_channel));                                \
            count += BM_MESSAGE_SIZE;                                                   \
        }                                                                               \
    }                                                                                   \
    K_THREAD_DEFINE(name##_id, BM_MESSAGE_SIZE + 196, name##_task, NULL, NULL, NULL, 3, \
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

void s_cb(zbus_chan_idx_t idx);

ZBUS_LISTENER_DECLARE(s1, s_cb);
#if (BM_ONE_TO >= 2LLU)
ZBUS_LISTENER_DECLARE(s2, s_cb);
#if (BM_ONE_TO > 2LLU)
ZBUS_LISTENER_DECLARE(s3, s_cb);
ZBUS_LISTENER_DECLARE(s4, s_cb);
#if (BM_ONE_TO > 4LLU)
ZBUS_LISTENER_DECLARE(s5, s_cb);
ZBUS_LISTENER_DECLARE(s6, s_cb);
ZBUS_LISTENER_DECLARE(s7, s_cb);
ZBUS_LISTENER_DECLARE(s8, s_cb);
#if (BM_ONE_TO > 8LLU)
ZBUS_LISTENER_DECLARE(s9, s_cb);
ZBUS_LISTENER_DECLARE(s10, s_cb);
ZBUS_LISTENER_DECLARE(s11, s_cb);
ZBUS_LISTENER_DECLARE(s12, s_cb);
ZBUS_LISTENER_DECLARE(s13, s_cb);
ZBUS_LISTENER_DECLARE(s14, s_cb);
ZBUS_LISTENER_DECLARE(s15, s_cb);
ZBUS_LISTENER_DECLARE(s16, s_cb);
#endif
#endif
#endif
#endif

struct bm_msg msg_received = {0};

void s_cb(zbus_chan_idx_t idx)
{
    struct external_data_msg *actual_message_data = NULL;
    zbus_chan_claim(ZBUS_CHAN_GET(bm_channel), K_NO_WAIT);
    actual_message_data = (struct external_data_msg *) ZBUS_CHAN_GET(bm_channel)->message;
    memcpy(&msg_received, actual_message_data->reference, sizeof(struct bm_msg));
    zbus_chan_finish(ZBUS_CHAN_GET(bm_channel));
    count += BM_MESSAGE_SIZE;
}

#endif  // BM_ASYNC

void main(void)
{
    printk(
        " Benchmark 1 to %lld: Dynamic memory, %sSYNC transmission and message size %u\n",
        BM_ONE_TO, BM_ASYNC ? "A" : "", BM_MESSAGE_SIZE);
}

void producer_thread(void)
{
    struct bm_msg msg = {0};
    for (uint64_t i = (BM_MESSAGE_SIZE - 1); i > 0; --i) {
        msg.bytes[i] = i;
    }

    struct external_data_msg *actual_message_data = NULL;
    zbus_chan_claim(ZBUS_CHAN_GET(bm_channel), K_NO_WAIT);
    actual_message_data = (struct external_data_msg *) ZBUS_CHAN_GET(bm_channel)->message;
    actual_message_data->reference = k_malloc(sizeof(struct bm_msg));
    actual_message_data->size      = sizeof(struct bm_msg);
    ZBUS_ASSERT(actual_message_data->reference != NULL);
    ZBUS_ASSERT(actual_message_data->size > 0);
    zbus_chan_finish(ZBUS_CHAN_GET(bm_channel));

    uint32_t start = k_uptime_get_32();
    for (uint64_t internal_count = BYTES_TO_BE_SENT / BM_ONE_TO; internal_count > 0;
         internal_count -= BM_MESSAGE_SIZE) {
        zbus_chan_claim(ZBUS_CHAN_GET(bm_channel), K_NO_WAIT);
        actual_message_data =
            (struct external_data_msg *) ZBUS_CHAN_GET(bm_channel)->message;
        memcpy(actual_message_data->reference, &msg, BM_MESSAGE_SIZE);
        zbus_chan_finish(ZBUS_CHAN_GET(bm_channel));
        zbus_chan_notify(ZBUS_CHAN_GET(bm_channel), K_MSEC(200));
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
    printk("\n@%u\n", duration);
}

K_THREAD_DEFINE(producer_thread_id, 1024, producer_thread, NULL, NULL, NULL, 5, 0, 5000);

void arch_system_halt(unsigned int reason)
{
    ARG_UNUSED(reason);

    /* TODO: What's the best way to totally halt the system if SMP
     * is enabled?
     */
    printk("Reason %d\n", reason);
    (void) arch_irq_lock();
    for (;;) {
        /* Spin endlessly */
    }
}
