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
LOG_MODULE_REGISTER(zbus);
K_MSGQ_DEFINE(__zt_channels_changed_msgq, sizeof(zt_channel_index_t), 32, 2);


#ifdef ZT_CHANNEL
#undef ZT_CHANNEL
#endif
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    K_SEM_DEFINE(__zt_sem_##name, 0, 1);
#include "zbus_channels.def"

// #undef ZT_CHANNEL
// #define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers,
// init_val) \
//     extern struct k_sem __zt_sem_##name;
// #include "zbus_channels.def"

// #undef ZT_CHANNEL
// #define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers,
// init_val) \
//     struct { \
//         struct metadata __zt_meta_##name; \
//         type name; \
//     };
// struct zt_channels {
// #include "zbus_channels.def"
// } __zt_channels = {
// #undef ZT_CHANNEL
// #define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers,
// init_val) \
//     .__zt_meta_##name = \
//         {#name,               /* Name */ \
//          .flag = {false,      /* Not defined yet */ \
//                   on_changed, /* Only changes in the channel will propagate  */ \
//                   read_only,  /* The channel is only for reading. It must have a
//                   initial \
//                                  value. */ \
//                   false},     /* ISC source flag */ \
//          zt_index_##name,     /* Lookup table index */ \
//          sizeof(type),        /* The channel's size */ \
//          (uint8_t *) &__zt_channels.name, /* The actual channel */ \
//          &__zt_sem_##name,                /* Channel's semaphore */ \
//          subscribers},                    /* List of subscribers queues */ \
//         .name = init_val,
// #include "zbus_channels.def"
// };

struct metadata *__zt_channels_lookup_table[] = {
#undef ZT_CHANNEL
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    &__zt_channels.__zt_meta_##name,
#include "zbus_channels.def"
};


struct zt_channels *zt_channels_instance()
{
    return &__zt_channels;
}

int __zt_sem_take(void *semaphore)
{
    return 0;
}

int __zt_sem_give(void *semaphore)
{
    return 0;
}


#define zt_chan_pub(chan, value)                                                         \
    ({                                                                                   \
        {                                                                                \
            __typeof__(__zt_channels.chan) chan##__aux__;                                \
            __typeof__(value) value##__aux__;                                            \
            (void) (&chan##__aux__ == &value##__aux__);                                  \
        }                                                                                \
        __zt_chan_pub(ZT_CHANNEL_METADATA_GET(chan), (uint8_t *) &value, sizeof(value)); \
    })

int __zt_chan_pub(struct metadata *meta, uint8_t *data, size_t data_size)
{
    __label__ cleanup;
    if ((meta->channel == NULL) || (data == NULL) || (data_size == 0)) {
        return -1;
    }
    if (meta->channel_size != data_size) {
        return -2;
    }
    ZT_CHECK(k_sem_take(meta->semaphore, K_MSEC(200)) != 0, -EBUSY,
             "Could not publish the channel. Channel is busy");
    if (meta->flag.on_changed) {  // CHANGE
        if (memcmp(meta->channel, data, data_size) == 0) {
            /* This data is not different from the channel's. No changes here. */
            meta->flag.pend_callback = false;
            goto cleanup;
        }
    }
#if defined(CONFIG_ZETA_SERIAL_IPC)
    if (k_current_get() == zt_serial_ipc_thread()) {
        meta->flag.field.source_serial_isc = 1;
    }
#endif
    memcpy(meta->channel, data, data_size);
    meta->flag.pend_callback = true;
    int error                = k_msgq_put(&__zt_channels_changed_msgq,
                           (uint8_t *) &meta->lookup_table_index, K_MSEC(500));
    if (error != 0) {
        LOG_INF("[Channel #%d] Error sending channels change message to ZT "
                "thread!",
                meta->lookup_table_index);
    }
cleanup:
    k_sem_give(meta->semaphore);
    return 0;
}


#define zt_chan_read(chan, value)                                         \
    ({                                                                    \
        {                                                                 \
            __typeof__(__zt_channels.chan) chan##__aux__;                 \
            __typeof__(value) value##__aux__;                             \
            (void) (&chan##__aux__ == &value##__aux__);                   \
        }                                                                 \
        __zt_chan_read(ZT_CHANNEL_METADATA_GET(chan), (uint8_t *) &value, \
                       sizeof(value));                                    \
    })

int __zt_chan_read(struct metadata *meta, uint8_t *data, size_t data_size)
{
    if ((meta->channel == NULL) || (data == NULL) || (data_size == 0)) {
        return -1;
    }
    if (meta->channel_size != data_size) {
        return -2;
    }
    // ZT_CHECK(k_sem_take(meta->semaphore, K_MSEC(200)) != 0, -EBUSY,
    //          "Could not read the channel. Channel is busy");
    memcpy(data, meta->channel, meta->channel_size);
    return 0;
}
static void __zt_monitor_thread(void)
{
    /* Initialization scope */ {
        struct zt_channels tmp__zt_channels = {
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
        memcpy(&__zt_channels, &tmp__zt_channels, sizeof(struct zt_channels));
    }
    zt_channel_index_t idx = 0;
    while (1) {
        k_msgq_get(&__zt_channels_changed_msgq, &idx, K_FOREVER);
        if (idx < ZT_CHANNEL_COUNT) {
            struct metadata *meta = __zt_channels_lookup_table[idx];
            if (meta->flag.pend_callback) {
                struct k_msgq **cursor = meta->subscribers;
                for (struct k_msgq *s = *cursor; s != NULL; ++cursor, s = *cursor) {
                    k_msgq_put(s, &idx, K_MSEC(50));
                    // (*s)->cb(idx);
                }

#if defined(CONFIG_ZBUS_SERIAL_IPC)
                if (meta->flag.source_serial_isc == 1) {
                    meta->flag.source_serial_isc = 0;
                } else {
                    zt_serial_ipc_send_update_to_host(idx);
                }
#endif
                meta->flag.pend_callback = false;

            } else {
                LOG_INF("[ZT-THREAD]: Received pend_callback from a channel(#%d) "
                        "without changes!",
                        idx);
            }
        } else {
            LOG_INF("[ZT-THREAD]: Received an invalid ID channel #%d", idx);
        }
    }
}

K_THREAD_DEFINE(zt_monitor_thread_id, CONFIG_ZBUS_MONITOR_THREAD_STACK_SIZE,
                __zt_monitor_thread, NULL, NULL, NULL,
                CONFIG_ZBUS_MONITOR_THREAD_PRIORITY, 0, 0);
