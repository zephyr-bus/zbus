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
K_MSGQ_DEFINE(__zbus_channels_changed_msgq, sizeof(zbus_channel_index_t), 32, 2);

#if defined(CONFIG_ZBUS_EXT)
K_MSGQ_DEFINE(__zbus_ext_msgq, sizeof(zbus_channel_index_t), 32, 2);
#endif

#ifdef ZBUS_CHANNEL
#undef ZBUS_CHANNEL
#endif

/**
 * @def ZBUS_CHANNEL
 * Description
 */
#define ZBUS_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, \
                     init_val)                                                   \
    K_SEM_DEFINE(__zbus_sem_##name, 1, 1);
#include "zbus_channels.h"

/**
 * @def ZBUS_CHANNEL_SUBSCRIBERS
 * Description
 */
#define ZBUS_CHANNEL_SUBSCRIBERS(sub_ref, ...) \
    extern struct zbus_subscriber sub_ref, ##__VA_ARGS__

#define ZBUS_CHANNEL_SUBSCRIBERS_EMPTY
#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, \
                     init_val)                                                   \
    subscribers;

#include "zbus_channels.h"

/**
 * @def ZBUS_REF
 * Description
 */
#define ZBUS_REF(a) &a

#undef ZBUS_CHANNEL_SUBSCRIBERS
#define ZBUS_CHANNEL_SUBSCRIBERS(...)                        \
    (struct zbus_subscriber **) (struct zbus_subscriber *[]) \
    {                                                        \
        FOR_EACH(ZBUS_REF, (, ), __VA_ARGS__), NULL          \
    }
#undef ZBUS_CHANNEL_SUBSCRIBERS_EMPTY
#define ZBUS_CHANNEL_SUBSCRIBERS_EMPTY                       \
    (struct zbus_subscriber **) (struct zbus_subscriber *[]) \
    {                                                        \
        NULL                                                 \
    }

static struct zbus_channels __zbus_channels = {

#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, persistant, on_changed, read_only, type, subscribers,        \
                     init_val)                                                          \
    .__zbus_meta_##name =                                                               \
        {.flag =                                                                        \
             {                                                                          \
                 false,      /* Not defined yet */                                      \
                 on_changed, /* Only changes in the channel will propagate  */          \
                 read_only,  /* The channel is only for reading. It must have a initial \
                                value. */                                               \
                 false       /* ISC source flag */                                      \
             },              /* ISC source flag */                                      \
         zbus_index_##name,  /* Lookup table index */                                   \
         sizeof(type),       /* The channel's size */                                   \
         (uint8_t *) &__zbus_channels.name, /* The actual channel */                    \
         &__zbus_sem_##name,                /* Channel's semaphore */                   \
         subscribers},                      /* List of subscribers queues */            \
        .name = init_val,

#include "zbus_channels.h"
};

struct metadata *__zbus_channels_lookup_table[] = {
#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, \
                     init_val)                                                   \
    &__zbus_channels.__zbus_meta_##name,
#include "zbus_channels.h"
};


/**
 * @brief Change the enable flag of the subscriber.
 *
 * @param sub the subscriber to changed
 * @param enabled if true, the Event notifier will send notification for this subscribe.
 * If false the Event dispatcher won't send notifications to this subscriber.
 */
void zbus_subscriber_set_enable(struct zbus_subscriber *sub, bool enabled)
{
    if (sub != NULL) {
        sub->enabled = enabled;
    }
}

/**
 * @brief This function returns the __zbus_channels instance reference.
 * @details Do not use this directly! It is being used by the auxilary functions.
 * @return A pointer of struct zbus_channels.
 */
struct zbus_channels *__zbus_channels_instance()
{
    return &__zbus_channels;
}


/**
 * @brief Retreives the channel's metada by a given index
 *
 * @param idx channel's index based on the generated enum.
 * @return the metada struct of the channel
 */
struct metadata *__zbus_metadata_get_by_id(zbus_channel_index_t idx)
{
    ZBUS_ASSERT(idx < ZBUS_CHANNEL_COUNT);
    return __zbus_channels_lookup_table[idx];
}


/**
 * @brief This function prints the channels information in json format. I would help if
 * the developer needs to decode information. Take a look at the uart_bridge sample to get
 * the idea.
 */
void zbus_info_dump(void)
{
    printk("[\n");
#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, persistant, on_changed, read_only, type, subscribers,        \
                     init_val)                                                          \
    printk("{\"name\":\"%s\",\"on_changed\": %s, \"read_only\": %s, \"message_size\": " \
           "%u},\n",                                                                    \
           #name, on_changed ? "true" : "false", read_only ? "true" : "false",          \
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
int __zbus_chan_pub(struct metadata *meta, uint8_t *msg, size_t msg_size,
                    k_timeout_t timeout, bool from_ext)
{
    ZBUS_ASSERT(meta != NULL);
    ZBUS_ASSERT(meta->flag.read_only == 0);
    ZBUS_ASSERT(meta->message != NULL);
    ZBUS_ASSERT(msg != NULL);
    ZBUS_ASSERT(msg_size > 0);

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
    ZBUS_ASSERT(meta->message_size == msg_size);
    memcpy(meta->message, msg, meta->message_size);
    meta->flag.pend_callback = true;
    meta->flag.from_ext      = from_ext;
    k_sem_give(meta->semaphore);
    return k_msgq_put(&__zbus_channels_changed_msgq,
                      (uint8_t *) &meta->lookup_table_index, timeout);
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
int __zbus_chan_read(struct metadata *meta, uint8_t *msg, size_t msg_size,
                     k_timeout_t timeout)
{
    ZBUS_ASSERT(meta != NULL);
    ZBUS_ASSERT(meta->message != NULL);
    ZBUS_ASSERT(msg != NULL);
    ZBUS_ASSERT(msg_size > 0);
    ZBUS_ASSERT(meta->message_size == msg_size);

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

int zbus_chan_notify(struct metadata *meta, k_timeout_t timeout)
{
    ZBUS_ASSERT(meta != NULL);
    ZBUS_ASSERT(meta->message != NULL);
    /* Force to not use timeout inside ISR */
    if (k_is_in_isr()) {
        timeout = K_NO_WAIT;
    }

    int err = k_sem_take(meta->semaphore, timeout);
    if (err < 0) {
        return err;
    }
    meta->flag.pend_callback = true;
    meta->flag.from_ext      = false;
    k_sem_give(meta->semaphore);
    return k_msgq_put(&__zbus_channels_changed_msgq,
                      (uint8_t *) &meta->lookup_table_index, timeout);
}

int zbus_chan_borrow(struct metadata *meta, void **chan_msg, k_timeout_t timeout)
{
    ZBUS_ASSERT(meta != NULL);
    int err = k_sem_take(meta->semaphore, timeout);
    if (err < 0) {
        return err;
    }
    *chan_msg = (void *) meta->message;
    return 0;
}

void zbus_chan_give_back(struct metadata *meta, k_timeout_t timeout)
{
    ZBUS_ASSERT(meta != NULL);
    k_sem_give(meta->semaphore);
}


#if defined(CONFIG_ZBUS_SERIAL_IPC)
K_MSGQ_DEFINE(__zbus_bridge_queue, sizeof(zbus_channel_index_t), 16, 2);
#endif

static void __zbus_monitor_thread(void)
{
    zbus_channel_index_t idx = 0;
    while (1) {
        k_msgq_get(&__zbus_channels_changed_msgq, &idx, K_FOREVER);
        ZBUS_ASSERT(idx < ZBUS_CHANNEL_COUNT);
        struct metadata *meta = __zbus_channels_lookup_table[idx];
        /*! If there are more than one change of the same channel, only the last one is
         * applied. */

        int err = k_sem_take(meta->semaphore,
                             K_MSEC(50)); /* Take control of meta, lock A lifetime */
        ZBUS_ASSERT(err == 0);            /* A'*/
        if (meta->flag.pend_callback) {   /* A'*/
#if defined(CONFIG_ZBUS_EXT)
            if (meta->flag.from_ext == false) {                 /* A'*/
                k_msgq_put(&__zbus_ext_msgq, &idx, K_MSEC(50)); /* A'*/
            }                                                   /* A'*/
#endif

            k_sem_give(meta->semaphore); /* Give control of meta, from lock A
                                                        lifetime */
            for (struct zbus_subscriber **sub = meta->subscribers; *sub != NULL; ++sub) {
                if ((*sub)->enabled) {
                    if ((*sub)->queue != NULL) {
                        k_msgq_put((*sub)->queue, &idx, K_MSEC(50));
                    } else if ((*sub)->callback != NULL) {
                        (*sub)->callback(idx);
                    }
                }
            }

            err = k_sem_take(meta->semaphore,
                             K_MSEC(1000));   /* Take control of meta, lock B lifetime */
            ZBUS_ASSERT(err == 0);            /* B'*/
            meta->flag.pend_callback = false; /* B'*/
            meta->flag.from_ext      = false; /* B'*/
            k_sem_give(meta->semaphore); /* Give control of meta, from lock B lifetime */

            __ZBUS_LOG_DBG("[ZBUS] notify!");
        }
    }
}

K_THREAD_DEFINE(zbus_monitor_thread_id, CONFIG_ZBUS_MONITOR_THREAD_STACK_SIZE,
                __zbus_monitor_thread, NULL, NULL, NULL,
                CONFIG_ZBUS_MONITOR_THREAD_PRIORITY, 0, 0);
