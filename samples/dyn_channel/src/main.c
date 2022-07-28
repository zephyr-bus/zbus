/*
 * Copyright (c) 2022 Citrinio
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "kernel.h"
#include "zbus.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

/*! benchmark description
 * data variation from 1 to 256 bytes (1,2,4,8,16,32,64,128,256)
 * callback based first
 */

struct pkt {
    struct {
        uint8_t filter : 1;
        uint8_t body_size : 4;

    } header;
    uint8_t body[];
};

void filter_cb(zbus_chan_idx_t idx);
ZBUS_LISTENER_DECLARE(filter, filter_cb);

union zbus_msg_var msg_received = {0};
void filter_cb(zbus_chan_idx_t idx)
{
    struct external_data_msg *chan_message = NULL;
    ZBUS_ASSERT(idx == pkt_channel_chan_idx);
    if (!zbus_chan_claim(ZBUS_CHAN_GET(pkt_channel), K_NO_WAIT)) {
        chan_message = (struct external_data_msg *) ZBUS_CHAN_GET(pkt_channel)->message;
        struct pkt *filtered_data = (struct pkt *) chan_message->reference;
        if (filtered_data->header.filter) {
            memset(filtered_data->body, 0, filtered_data->header.body_size);
        }
        zbus_chan_finish(ZBUS_CHAN_GET(pkt_channel));
    }
    struct ack_msg dr = {1};
    ZBUS_CHAN_PUB(data_ready, dr, K_NO_WAIT);
}

void main(void)
{
    struct version_msg v = {0};
    ZBUS_CHAN_READ(version, v, K_NO_WAIT);

    printk("\n -> Sample dynamic filter version %u.%u-%u\n\n", v.major, v.minor, v.build);
}

ZBUS_SUBSCRIBER_DECLARE(producer, 4);
void producer_thread(void)
{
    struct pkt *msg     = NULL;
    zbus_chan_idx_t idx = ZBUS_CHAN_COUNT;

    int i = 0;
    do {
        msg = k_malloc(i + 1);
        ZBUS_ASSERT(msg != NULL);
        memset(&msg->body, i, i);
        msg->header.body_size                = i;
        msg->header.filter                   = i % 2;
        struct external_data_msg malloc_data = {.reference = msg,
                                                .size      = sizeof(struct pkt) + i};
        ZBUS_CHAN_PUB(pkt_channel, malloc_data, K_NO_WAIT);
        ++i;
    } while ((i < 16) && !k_msgq_get(producer.queue, &idx, K_FOREVER));
}
K_THREAD_DEFINE(producer_thread_id, 1024, producer_thread, NULL, NULL, NULL, 5, 0, 5000);


ZBUS_SUBSCRIBER_DECLARE(consumer, 4);
void consumer_thread(void)
{
    struct external_data_msg *chan_message = NULL;
    zbus_chan_idx_t idx                    = ZBUS_CHAN_COUNT;
    while (!k_msgq_get(consumer.queue, &idx, K_FOREVER)) {
        ZBUS_ASSERT(idx == data_ready_chan_idx);
        if (!zbus_chan_claim(ZBUS_CHAN_GET(pkt_channel), K_NO_WAIT)) {
            chan_message =
                (struct external_data_msg *) ZBUS_CHAN_GET(pkt_channel)->message;
            struct pkt *received = (struct pkt *) chan_message->reference;
            printk("Header: filter=%d,body_size=%02d; Body: ", received->header.filter,
                   received->header.body_size);
            for (int i = 0; i < received->header.body_size; ++i) {
                printk("%02X", received->body[i]);
            }
            printk("\n");

            /* Cleanup the dynamic memory after using that */
            k_free(chan_message->reference);
            chan_message->reference = NULL;
            chan_message->size      = 0;
            zbus_chan_finish(ZBUS_CHAN_GET(pkt_channel));
        }
        struct ack_msg a = {1};
        ZBUS_CHAN_PUB(ack, a, K_MSEC(250));
    }
}

K_THREAD_DEFINE(consumer_thread_id, 1024, consumer_thread, NULL, NULL, NULL, 4, 0, 5000);
