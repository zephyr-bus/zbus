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
K_MSGQ_DEFINE(__zt_channels_changed_msgq, sizeof(zt_channel_index_t), 32, 2);

#ifdef ZT_CHANNEL
#undef ZT_CHANNEL
#endif

/**
 * @def ZT_CHANNEL
 * Description
 */
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    K_SEM_DEFINE(__zt_sem_##name, 1, 1);
#include "zbus_channels.def"

/**
 * @def ZT_CHANNEL_SUBSCRIBERS_QUEUES
 * Description
 */
#define ZT_CHANNEL_SUBSCRIBERS_QUEUES(sub_ref, ...) \
    extern struct k_msgq sub_ref, ##__VA_ARGS__

#define ZT_CHANNEL_HAS_NO_SUBSCRIBERS
#undef ZT_CHANNEL
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    subscribers;

#include "zbus_channels.def"

/**
 * @def ZT_REF
 * Description
 */
#define ZT_REF(a) &a

#undef ZT_CHANNEL_SUBSCRIBERS_QUEUES
#define ZT_CHANNEL_SUBSCRIBERS_QUEUES(...)        \
    (struct k_msgq **) (struct k_msgq *[])        \
    {                                             \
        FOR_EACH(ZT_REF, (, ), __VA_ARGS__), NULL \
    }
#undef ZT_CHANNEL_HAS_NO_SUBSCRIBERS
#define ZT_CHANNEL_HAS_NO_SUBSCRIBERS      \
    (struct k_msgq **) (struct k_msgq *[]) \
    {                                      \
        NULL                               \
    }

static struct zt_channels __zt_channels = {
#undef ZT_CHANNEL
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    .__zt_meta_##name =                                                                  \
        {#name,               /* Name */                                                 \
         .flag = {false,      /* Not defined yet */                                      \
                  on_changed, /* Only changes in the channel will propagate  */          \
                  read_only,  /* The channel is only for reading. It must have a initial \
                                 value. */                                               \
                  false},     /* ISC source flag */                                      \
         zt_index_##name,     /* Lookup table index */                                   \
         sizeof(type),        /* The channel's size */                                   \
         (uint8_t *) &__zt_channels.name, /* The actual channel */                       \
         &__zt_sem_##name,                /* Channel's semaphore */                      \
         subscribers},                    /* List of subscribers queues */               \
        .name = init_val,
#include "zbus_channels.def"
};

struct metadata *__zt_channels_lookup_table[] = {
#undef ZT_CHANNEL
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    &__zt_channels.__zt_meta_##name,
#include "zbus_channels.def"
};

/**
 * @brief This function returns the __zt_channels instance reference.
 * @details Do not use this directly! It is being used by the auxilary functions.
 * @return A pointer of struct zt_channels.
 */
struct zt_channels *__zt_channels_instance()
{
    return &__zt_channels;
}

int __zt_chan_pub(struct metadata *meta, uint8_t *data, size_t data_size)
{
    __label__ cleanup;
    int ret = 0;
    __ASSERT_NO_MSG(meta->channel != NULL);
    __ASSERT_NO_MSG(data != NULL);
    __ASSERT_NO_MSG(data_size > 0);
    __ASSERT_NO_MSG(meta->channel_size == data_size);
    __ASSERT_NO_MSG(!meta->flag.read_only);
    if (k_sem_take(meta->semaphore, K_MSEC(200))) {
        ret = -1;
        goto cleanup;
    }
    if (meta->flag.on_changed) {  // CHANGE
        if (memcmp(meta->channel, data, data_size) == 0) {
            /* This data is not different from the channel's. No changes here. */
            meta->flag.pend_callback = false;
            goto cleanup;
        }
    }
    memcpy(meta->channel, data, data_size);
    meta->flag.pend_callback = true;
    if (k_msgq_put(&__zt_channels_changed_msgq, (uint8_t *) &meta->lookup_table_index,
                   K_MSEC(500))) {
        ret = -1;
    }
cleanup:
    k_sem_give(meta->semaphore);
    return ret;
}


int __zt_chan_read(struct metadata *meta, uint8_t *data, size_t data_size)
{
    __label__ cleanup;
    int ret = 0;
    __ASSERT_NO_MSG(meta->channel != NULL);
    __ASSERT_NO_MSG(data != NULL);
    __ASSERT_NO_MSG(data_size > 0);
    __ASSERT_NO_MSG(meta->channel_size == data_size);
    if (k_sem_take(meta->semaphore, K_MSEC(200)) == 0) {
        ret = -1;
        goto cleanup;
    }
    memcpy(data, meta->channel, meta->channel_size);
cleanup:
    k_sem_give(meta->semaphore);
    return ret;
}

#if defined(CONFIG_ZBUS_SERIAL_IPC)
K_MSGQ_DEFINE(__zt_bridge_queue, sizeof(zt_channel_index_t), 16, 2);
#endif

static void __zt_monitor_thread(void)
{
    zt_channel_index_t idx = 0;
    while (1) {
        k_msgq_get(&__zt_channels_changed_msgq, &idx, K_FOREVER);
        __ASSERT_NO_MSG(idx < ZT_CHANNEL_COUNT);
        struct metadata *meta = __zt_channels_lookup_table[idx];
        __ASSERT_NO_MSG(meta->flag.pend_callback);
#if defined(CONFIG_ZBUS_SERIAL_IPC)
        k_msgq_put(&__zt_bridge_queue, &idx, K_MSEC(50));
#endif
        struct k_msgq **cursor = meta->subscribers;
        for (struct k_msgq *s = *cursor; s != NULL; ++cursor, s = *cursor) {
            k_msgq_put(s, &idx, K_MSEC(50));
        }
        meta->flag.pend_callback = false;
    }
}

K_THREAD_DEFINE(zt_monitor_thread_id, CONFIG_ZBUS_MONITOR_THREAD_STACK_SIZE,
                __zt_monitor_thread, NULL, NULL, NULL,
                CONFIG_ZBUS_MONITOR_THREAD_PRIORITY, 0, 0);

#if defined(CONFIG_ZBUS_SERIAL_IPC)
void __zt_bridge_thread(void)
{
    zt_channel_index_t idx                              = 0;
    uint8_t data[CONFIG_ZBUS_IPC_BRIDGE_MAX_BUFFER_LEN] = {0};
    while (1) {
        if (!k_msgq_get(&__zt_bridge_queue, &idx, K_FOREVER)) {
            struct metadata *meta = __zt_channels_lookup_table[idx];
            if (k_sem_take(meta->semaphore, K_MSEC(200)) == 0) {
                memcpy(data, meta->channel, meta->channel_size);
                k_sem_give(meta->semaphore);
                // do what to do with the data
                memset(data, 0, CONFIG_ZBUS_IPC_BRIDGE_MAX_BUFFER_LEN);
            }
        }
    }
}

K_THREAD_DEFINE(zt_bridge_thread, 2048, __zt_bridge_thread, NULL, NULL, NULL,
                (CONFIG_ZBUS_MONITOR_THREAD_PRIORITY + 1), 0, 0);
#endif
