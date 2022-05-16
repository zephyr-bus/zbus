/**
 * @file      zbus.c
 * @brief     Header of
 * @date      Sun Nov 28 18:08:41 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 * This module
 */

#include "zbus.h"
#include <kernel.h>
#include <sys/printk.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(zbus, CONFIG_ZBUS_LOG_LEVEL);
K_MSGQ_DEFINE(__zb_channels_changed_msgq, sizeof(zb_channel_index_t), 32, 2);

#ifdef ZB_CHANNEL
#undef ZB_CHANNEL
#endif

/**
 * @def ZB_CHANNEL
 * Description
 */
#define ZB_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    K_SEM_DEFINE(__zb_sem_##name, 1, 1);
#include "zbus_channels.h"

/**
 * @def ZB_CHANNEL_SUBSCRIBERS_QUEUES
 * Description
 */
#define ZB_CHANNEL_SUBSCRIBERS_QUEUES(sub_ref, ...) \
    extern struct k_msgq sub_ref, ##__VA_ARGS__

#define ZB_CHANNEL_SUBSCRIBERS_QUEUES_EMPTY
#undef ZB_CHANNEL
#define ZB_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    subscribers;

#include "zbus_channels.h"

/**
 * @def ZB_REF
 * Description
 */
#define ZB_REF(a) &a

#undef ZB_CHANNEL_SUBSCRIBERS_QUEUES
#define ZB_CHANNEL_SUBSCRIBERS_QUEUES(...)        \
    (struct k_msgq **) (struct k_msgq *[])        \
    {                                             \
        FOR_EACH(ZB_REF, (, ), __VA_ARGS__), NULL \
    }
#undef ZB_CHANNEL_SUBSCRIBERS_QUEUES_EMPTY
#define ZB_CHANNEL_SUBSCRIBERS_QUEUES_EMPTY \
    (struct k_msgq **) (struct k_msgq *[])  \
    {                                       \
        NULL                                \
    }

static struct zb_channels __zb_channels = {
#undef ZB_CHANNEL
#define ZB_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    .__zb_meta_##name =                                                                  \
        {.flag = {false,      /* Not defined yet */                                      \
                  on_changed, /* Only changes in the channel will propagate  */          \
                  read_only,  /* The channel is only for reading. It must have a initial \
                                 value. */                                               \
                  false},     /* ISC source flag */                                      \
         zb_index_##name,     /* Lookup table index */                                   \
         sizeof(type),        /* The channel's size */                                   \
         (uint8_t *) &__zb_channels.name, /* The actual channel */                       \
         &__zb_sem_##name,                /* Channel's semaphore */                      \
         subscribers},                    /* List of subscribers queues */               \
        .name = init_val,
#include "zbus_channels.h"
};

struct metadata *__zb_channels_lookup_table[] = {
#undef ZB_CHANNEL
#define ZB_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    &__zb_channels.__zb_meta_##name,
#include "zbus_channels.h"
};

/**
 * @brief This function returns the __zb_channels instance reference.
 * @details Do not use this directly! It is being used by the auxilary functions.
 * @return A pointer of struct zb_channels.
 */
struct zb_channels *__zb_channels_instance()
{
    return &__zb_channels;
}

int __zb_chan_pub(struct metadata *meta, uint8_t *data, size_t data_size)
{
    __label__ cleanup;
    int ret = 0;
    ZB_ASSERT(meta->channel != NULL);
    ZB_ASSERT(data != NULL);
    ZB_ASSERT(data_size > 0);
    ZB_ASSERT(meta->channel_size == data_size);
    ZB_ASSERT(!meta->flag.read_only);
    if (k_sem_take(meta->semaphore, K_MSEC(200))) {
        ret = -1;
        goto cleanup;
    }
    if (meta->flag.on_changed) {  // CHANGE
        if (memcmp(meta->channel, data, data_size) == 0) {
            /* This data is not different from the channel's. No changes here. */
            goto cleanup;
        }
    }
    memcpy(meta->channel, data, data_size);
    meta->flag.pend_callback = true;
    if (k_msgq_put(&__zb_channels_changed_msgq, (uint8_t *) &meta->lookup_table_index,
                   K_MSEC(500))) {
        ret = -2;
    }
cleanup:
    k_sem_give(meta->semaphore);
    return ret;
}


int __zb_chan_read(struct metadata *meta, uint8_t *data, size_t data_size)
{
    __label__ cleanup;
    int ret = 0;
    ZB_ASSERT(meta->channel != NULL);
    ZB_ASSERT(data != NULL);
    ZB_ASSERT(data_size > 0);
    ZB_ASSERT(meta->channel_size == data_size);
    if (k_sem_take(meta->semaphore, K_MSEC(200))) {
        ret = -1;
        goto cleanup;
    }
    memcpy(data, meta->channel, meta->channel_size);
cleanup:
    k_sem_give(meta->semaphore);
    return ret;
}

#if defined(CONFIG_ZBUS_SERIAL_IPC)
K_MSGQ_DEFINE(__zb_bridge_queue, sizeof(zb_channel_index_t), 16, 2);
#endif

static void __zb_monitor_thread(void)
{
    zb_channel_index_t idx = 0;
    while (1) {
        k_msgq_get(&__zb_channels_changed_msgq, &idx, K_FOREVER);
        ZB_ASSERT(idx < ZB_CHANNEL_COUNT);
        struct metadata *meta = __zb_channels_lookup_table[idx];
        /*! If there are more than one change of the same channel, only the last one is
         * applied. */
        if (meta->flag.pend_callback) {
            LOG_INF("[ZBUS] notify!");
#if defined(CONFIG_ZBUS_SERIAL_IPC)
            k_msgq_put(&__zb_bridge_queue, &idx, K_MSEC(50));
#endif
            struct k_msgq **cursor = meta->subscribers;
            for (struct k_msgq *s = *cursor; s != NULL; ++cursor, s = *cursor) {
                k_msgq_put(s, &idx, K_MSEC(50));
            }
            meta->flag.pend_callback = false;
        }
    }
}

K_THREAD_DEFINE(zb_monitor_thread_id, CONFIG_ZBUS_MONITOR_THREAD_STACK_SIZE,
                __zb_monitor_thread, NULL, NULL, NULL,
                CONFIG_ZBUS_MONITOR_THREAD_PRIORITY, 0, 0);

#if defined(CONFIG_ZBUS_SERIAL_IPC)
void __zb_bridge_thread(void)
{
    zb_channel_index_t idx                              = 0;
    uint8_t data[CONFIG_ZBUS_IPC_BRIDGE_MAX_BUFFER_LEN] = {0};
    while (1) {
        if (!k_msgq_get(&__zb_bridge_queue, &idx, K_FOREVER)) {
            struct metadata *meta = __zb_channels_lookup_table[idx];
            if (k_sem_take(meta->semaphore, K_MSEC(200)) == 0) {
                memcpy(data, meta->channel, meta->channel_size);
                k_sem_give(meta->semaphore);
                // do what to do with the data
                memset(data, 0, CONFIG_ZBUS_IPC_BRIDGE_MAX_BUFFER_LEN);
            }
        }
    }
}

K_THREAD_DEFINE(zb_bridge_thread, 2048, __zb_bridge_thread, NULL, NULL, NULL,
                (CONFIG_ZBUS_MONITOR_THREAD_PRIORITY + 1), 0, 0);
#endif
