/**
 * @file      zbus.c
 * @brief     Header of
 * @date      Sun Nov 28 18:08:41 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 */
#include "zbus.h"
#include <kernel.h>
#include <sys/printk.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(zbus, CONFIG_ZBUS_LOG_LEVEL);
K_MSGQ_DEFINE(__zb_channels_changed_msgq, sizeof(zb_channel_index_t), 32, 2);

#if defined(CONFIG_ZBUS_EXT)
K_MSGQ_DEFINE(__zb_ext_msgq, sizeof(zb_channel_index_t), 32, 2);
#endif

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
 * @def ZB_CHANNEL_SUBSCRIBERS
 * Description
 */
#define ZB_CHANNEL_SUBSCRIBERS(sub_ref, ...) \
    extern struct zb_subscriber sub_ref, ##__VA_ARGS__

#define ZB_CHANNEL_SUBSCRIBERS_EMPTY
#undef ZB_CHANNEL
#define ZB_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    subscribers;

#include "zbus_channels.h"

/**
 * @def ZB_REF
 * Description
 */
#define ZB_REF(a) &a

#undef ZB_CHANNEL_SUBSCRIBERS
#define ZB_CHANNEL_SUBSCRIBERS(...)                      \
    (struct zb_subscriber **) (struct zb_subscriber *[]) \
    {                                                    \
        FOR_EACH(ZB_REF, (, ), __VA_ARGS__), NULL        \
    }
#undef ZB_CHANNEL_SUBSCRIBERS_EMPTY
#define ZB_CHANNEL_SUBSCRIBERS_EMPTY                     \
    (struct zb_subscriber **) (struct zb_subscriber *[]) \
    {                                                    \
        NULL                                             \
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
 * @brief Change the enable flag of the subscriber.
 *
 * @param sub the subscriber to changed
 * @param enabled if true, the Event notifier will send notification for this subscribe.
 * If false the Event dispatcher won't send notifications to this subscriber.
 */
void zb_subscriber_set_enable(struct zb_subscriber *sub, bool enabled)
{
    if (sub != NULL) {
        sub->enabled = enabled;
    }
}

/**
 * @brief This function returns the __zb_channels instance reference.
 * @details Do not use this directly! It is being used by the auxilary functions.
 * @return A pointer of struct zb_channels.
 */
struct zb_channels *__zb_channels_instance()
{
    return &__zb_channels;
}


/**
 * @brief Retreives the channel's metada by a given index
 *
 * @param idx channel's index based on the generated enum.
 * @return the metada struct of the channel
 */
struct metadata *__zb_metadata_get_by_id(zb_channel_index_t idx)
{
    ZB_ASSERT(idx < ZB_CHANNEL_COUNT);
    return __zb_channels_lookup_table[idx];
}


/**
 * @brief This function prints the channels information in json format. I would help if
 * the developer needs to decode information. Take a look at the uart_bridge sample to get
 * the idea.
 */
void zb_info_dump(void)
{
    printk("[\n");
#undef ZB_CHANNEL
#define ZB_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    printk("{\"name\":\"%s\",\"on_changed\": %s, \"read_only\": %s, \"message_size\": "  \
           "%u},\n",                                                                     \
           #name, on_changed ? "true" : "false", read_only ? "true" : "false",           \
           sizeof(type));
#include "zbus_channels.h"
    printk("\n]\n");
}

/**
 * @brief Channel publish function.
 * This function publishes data to a channel. This function must not be called directly.
 *
 * @param meta Channel's metadata.
 * @param data The message data to be written to the channel. This must be the same type
 * as the channel.
 * @param data_size The size of the type.
 * @param timeout The timeout for the operation to fail. If you are calling this from a
 * ISR it will force timeout to be K_NO_WAIT. The pub function can use the timeout twice,
 * once for taking the semaphore and another to put the idx at the monitor's queue.
 * @return 0 if succes and a negative number if error.
 */
int __zb_chan_pub(struct metadata *meta, uint8_t *msg, size_t msg_size,
                  k_timeout_t timeout, bool from_ext)
{
    ZB_ASSERT(meta != NULL);
    ZB_ASSERT(meta->message != NULL);
    ZB_ASSERT(msg != NULL);
    ZB_ASSERT(msg_size > 0);
    ZB_ASSERT(meta->message_size == msg_size);
    ZB_ASSERT(!meta->flag.read_only);

    /* Force to not use timeout inside ISR */
    if (k_is_in_isr()) {
        timeout = K_NO_WAIT;
    }

    int err = k_sem_take(meta->semaphore, timeout);
    if (err < 0) {
        return err;
    }
    if (meta->flag.on_changed) {  // CHANGE
        if (memcmp(meta->message, msg, meta->message_size) == 0) {
            /* This data is not different from the channel's. No changes here. */
            k_sem_give(meta->semaphore);
            return 0;
        }
    }
    memcpy(meta->message, msg, meta->message_size);
    meta->flag.pend_callback = true;
    meta->flag.from_ext      = from_ext;
    k_sem_give(meta->semaphore);
    return k_msgq_put(&__zb_channels_changed_msgq, (uint8_t *) &meta->lookup_table_index,
                      timeout);
}


/**
 * @brief Channel read function.
 * This function enables the reading of a channel message. This function must not be
 * called directly.
 *
 * @param meta Channel's metadata.
 * @param data The message data to be read from the channel. This must be the same type
 * as the channel.
 * @param data_size The size of the type.
 * @param timeout The timeout for the operation to fail. If you are calling this from a
 * ISR it will force the timeout to be K_NO_WAIT.
 * @return 0 if succes and a negative number if error.
 */
int __zb_chan_read(struct metadata *meta, uint8_t *msg, size_t msg_size,
                   k_timeout_t timeout)
{
    ZB_ASSERT(meta != NULL);
    ZB_ASSERT(meta->message != NULL);
    ZB_ASSERT(msg != NULL);
    ZB_ASSERT(msg_size > 0);
    ZB_ASSERT(meta->message_size == msg_size);

    /* Force to not use timeout inside ISR */
    if (k_is_in_isr()) {
        timeout = K_NO_WAIT;
    }

    int err = k_sem_take(meta->semaphore, timeout);
    if (err < 0) {
        return err;
    }
    memcpy(msg, meta->message, meta->message_size);
    k_sem_give(meta->semaphore);
    return err;
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

        int err = k_sem_take(meta->semaphore,
                             K_MSEC(50)); /* Take control of meta, lock A lifetime */
        ZB_ASSERT(err >= 0);              /* A'*/
        if (meta->flag.pend_callback) {   /* A'*/
#if defined(CONFIG_ZBUS_EXT)
            if (meta->flag.from_ext == false) {               /* A'*/
                k_msgq_put(&__zb_ext_msgq, &idx, K_MSEC(50)); /* A'*/
            }                                                 /* A'*/
#endif

            for (struct zb_subscriber **sub = meta->subscribers; *sub != NULL; ++sub) {
                if ((*sub)->enabled) {
                    if ((*sub)->queue != NULL) {
                        k_msgq_put((*sub)->queue, &idx, K_MSEC(50));
                    } else if ((*sub)->callback != NULL) {
                        k_sem_give(meta->semaphore); /* Give control of meta, from lock A
                                                        lifetime */
                        (*sub)->callback(idx);
                        err = k_sem_take(
                            meta->semaphore,
                            K_MSEC(50)); /* Take control of meta, lock B lifetime */
                    }
                }
            }

            ZB_ASSERT(err >= 0);              /* B'*/
            meta->flag.pend_callback = false; /* B'*/
            meta->flag.from_ext      = false; /* B'*/
            k_sem_give(meta->semaphore); /* Give control of meta, from lock B lifetime */

            __ZB_LOG_DBG("[ZBUS] notify!");
        }
    }
}

K_THREAD_DEFINE(zb_monitor_thread_id, CONFIG_ZBUS_MONITOR_THREAD_STACK_SIZE,
                __zb_monitor_thread, NULL, NULL, NULL,
                CONFIG_ZBUS_MONITOR_THREAD_PRIORITY, 0, 0);
