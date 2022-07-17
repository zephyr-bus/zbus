/**
 * @file      zbus.c
 * @brief     Header of
 * @date      Sun Nov 28 18:08:41 2021
 * @author    Rodrigo Peixoto
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "zbus.h"
#include <kernel.h>
#include <sys/printk.h>

#include <logging/log.h>

LOG_MODULE_REGISTER(zbus, CONFIG_ZBUS_LOG_LEVEL);
K_MSGQ_DEFINE(_zbus_channels_changed_msgq, sizeof(zbus_channel_index_t), 32, 2);

#if defined(CONFIG_ZBUS_EXT)
K_MSGQ_DEFINE(_zbus_ext_msgq, sizeof(zbus_channel_index_t), 32, 2);
#endif

#ifdef ZBUS_CHANNEL
#undef ZBUS_CHANNEL
#endif

/**
 * @def ZBUS_CHANNEL
 * Description
 */
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    K_SEM_DEFINE(_zbus_sem_##name, 1, 1);

#include "zbus_channels.h"

/**
 * @def ZBUS_OBSERVERS
 * Description
 */
#define ZBUS_OBSERVERS(sub_ref, ...) extern struct zbus_observer sub_ref, ##__VA_ARGS__

#define ZBUS_OBSERVERS_EMPTY
#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    observers;

#include "zbus_channels.h"

/**
 * @def ZBUS_REF
 * Description
 */
#define ZBUS_REF(a) &a

#undef ZBUS_OBSERVERS
#define ZBUS_OBSERVERS(...)                              \
    (struct zbus_observer **) (struct zbus_observer *[]) \
    {                                                    \
        FOR_EACH(ZBUS_REF, (, ), __VA_ARGS__), NULL      \
    }
#undef ZBUS_OBSERVERS_EMPTY
#define ZBUS_OBSERVERS_EMPTY                             \
    (struct zbus_observer **) (struct zbus_observer *[]) \
    {                                                    \
        NULL                                             \
    }

static struct zbus_messages _zbus_messages = {

#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    .name = init_val,

#include "zbus_channels.h"
};

static struct zbus_channels _zbus_channels = {

#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    ._zbus_chan_##name = {                                                             \
        .flag =                                                                         \
            {                                                                           \
                false,      /* Not defined yet */                                       \
                on_changed, /* Only changes in the channel will propagate  */           \
                read_only,  /* The channel is only for reading. It must have a initial  \
                               value. */                                                \
                false       /* ISC source flag */                                       \
            },              /* ISC source flag */                                       \
        name##_index,       /* Lookup table index */                                    \
        sizeof(type),       /* The channel's size */                                    \
        (uint8_t *) &_zbus_messages.name, /* The actual channel */                     \
        validator,          /* The channel's message validator function */              \
        &_zbus_sem_##name, /* Channel's semaphore */                                   \
        observers},         /* List of observers queues */

#include "zbus_channels.h"
};

struct zbus_channel *zbus_channels_lookup_table[] = {
#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    &_zbus_channels._zbus_chan_##name,

#include "zbus_channels.h"
};


/**
 * @brief Change the enable flag of the subscriber.
 *
 * @param sub the subscriber to changed
 * @param enabled if true, the Event notifier will send notification for this subscribe.
 * If false the Event dispatcher won't send notifications to this subscriber.
 */
void zbus_observer_set_enable(struct zbus_observer *sub, bool enabled)
{
    if (sub != NULL) {
        sub->enabled = enabled;
    }
}

struct zbus_messages *zbus_messages_instance()
{
    return &_zbus_messages;
}

struct zbus_channels *zbus_channels_instance()
{
    return &_zbus_channels;
}

struct zbus_channel *zbus_chan_get_by_index(zbus_channel_index_t idx)
{
    ZBUS_ASSERT(idx < ZBUS_CHANNEL_COUNT);
    return zbus_channels_lookup_table[idx];
}

void zbus_info_dump(void)
{
    printk("[\n");
#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, on_changed, read_only, type, validator, observers, init_val) \
    printk("{\"name\":\"%s\",\"on_changed\": %s, \"read_only\": %s, \"message_size\": " \
           "%u},\n",                                                                    \
           #name, (on_changed) ? "true" : "false", (read_only) ? "true" : "false",          \
           (uint32_t) sizeof(type));

#include "zbus_channels.h"

    printk("\n]\n");
}

int zbus_chan_pub(struct zbus_channel *chan, uint8_t *msg, size_t msg_size,
                  k_timeout_t timeout, bool from_ext)
{
    ZBUS_ASSERT(chan != NULL);
    ZBUS_ASSERT(chan->flag.read_only == 0);
    ZBUS_ASSERT(chan->message != NULL);
    ZBUS_ASSERT(msg != NULL);
    ZBUS_ASSERT(msg_size > 0);
    if (chan->validator != NULL) {
        if (chan->validator(msg, chan->message_size) == false) {
            return -1;
        }
    }

    /* Force to not use timeout inside ISR */
    if (k_is_in_isr()) {
        timeout = K_NO_WAIT;
    }

    int err = k_sem_take(chan->semaphore, timeout);
    if (err < 0) {
        return err;
    }
    if (chan->flag.on_changed) {  // CHANGE
        if (memcmp(chan->message, msg, chan->message_size) == 0) {
            /* This data is not different from the channel's. No changes here. */
            k_sem_give(chan->semaphore);
            return 0;
        }
    }
    ZBUS_ASSERT(chan->message_size == msg_size);
    memcpy(chan->message, msg, chan->message_size);
    chan->flag.pend_callback = true;
    chan->flag.from_ext      = from_ext;
    k_sem_give(chan->semaphore);
    return k_msgq_put(&_zbus_channels_changed_msgq,
                      (uint8_t *) &chan->lookup_table_index, timeout);
}

int zbus_chan_read(struct zbus_channel *chan, uint8_t *msg, size_t msg_size,
                   k_timeout_t timeout)
{
    ZBUS_ASSERT(chan != NULL);
    ZBUS_ASSERT(chan->message != NULL);
    ZBUS_ASSERT(msg != NULL);
    ZBUS_ASSERT(msg_size > 0);
    ZBUS_ASSERT(chan->message_size == msg_size);

    /* Force to not use timeout inside ISR */
    if (k_is_in_isr()) {
        timeout = K_NO_WAIT;
    }

    int err = k_sem_take(chan->semaphore, timeout);
    if (err < 0) {
        return err;
    }
    memcpy(msg, chan->message, chan->message_size);
    k_sem_give(chan->semaphore);
    return err;
}

int zbus_chan_notify(struct zbus_channel *chan, k_timeout_t timeout)
{
    ZBUS_ASSERT(chan != NULL);
    ZBUS_ASSERT(chan->message != NULL);
    /* Force to not use timeout inside ISR */
    if (k_is_in_isr()) {
        timeout = K_NO_WAIT;
    }

    int err = k_sem_take(chan->semaphore, timeout);
    if (err < 0) {
        return err;
    }
    chan->flag.pend_callback = true;
    chan->flag.from_ext      = false;
    k_sem_give(chan->semaphore);
    return k_msgq_put(&_zbus_channels_changed_msgq,
                      (uint8_t *) &chan->lookup_table_index, timeout);
}

int zbus_chan_claim(struct zbus_channel *chan, void **chan_msg, k_timeout_t timeout)
{
    ZBUS_ASSERT(chan != NULL);
    int err = k_sem_take(chan->semaphore, timeout);
    if (err < 0) {
        return err;
    }
    *chan_msg = (void *) chan->message;
    return 0;
}

void zbus_chan_finish(struct zbus_channel *meta)
{
    ZBUS_ASSERT(meta != NULL);
    k_sem_give(meta->semaphore);
}

#if defined(CONFIG_ZBUS_SERIAL_IPC)
K_MSGQ_DEFINE(_zbus_bridge_queue, sizeof(zbus_channel_index_t), 16, 2);
#endif

_Noreturn static void zbus_monitor_thread(void)
{
    zbus_channel_index_t idx = 0;
    while (1) {
        k_msgq_get(&_zbus_channels_changed_msgq, &idx, K_FOREVER);
        ZBUS_ASSERT(idx < ZBUS_CHANNEL_COUNT);
        struct zbus_channel *chan = zbus_channels_lookup_table[idx];
        /*! If there are more than one change of the same channel, only the last one is
         * applied. */

        int err = k_sem_take(chan->semaphore,
                             K_MSEC(50)); /* Take control of chan, lock A lifetime */
        ZBUS_ASSERT(err == 0);            /* A'*/
        if (chan->flag.pend_callback) {   /* A'*/
#if defined(CONFIG_ZBUS_EXT)
            if (chan->flag.from_ext == false) {                 /* A'*/
                k_msgq_put(&_zbus_ext_msgq, &idx, K_MSEC(50)); /* A'*/
            }                                                   /* A'*/
#endif

            k_sem_give(chan->semaphore); /* Give control of chan, from lock A
                                                        lifetime */
            for (struct zbus_observer **sub = chan->observers; *sub != NULL; ++sub) {
                if ((*sub)->enabled) {
                    if ((*sub)->queue != NULL) {
                        k_msgq_put((*sub)->queue, &idx, K_MSEC(50));
                    } else if ((*sub)->callback != NULL) {
                        (*sub)->callback(idx);
                    }
                }
            }

            err = k_sem_take(chan->semaphore,
                             K_MSEC(1000));   /* Take control of chan, lock B lifetime */
            ZBUS_ASSERT(err == 0);            /* B'*/
            chan->flag.pend_callback = false; /* B'*/
            chan->flag.from_ext      = false; /* B'*/
            k_sem_give(chan->semaphore); /* Give control of chan, from lock B lifetime */

            ZBUS_LOG_DBG("[ZBUS] notify!");
        }
    }
}

K_THREAD_DEFINE(zbus_monitor_thread_id, CONFIG_ZBUS_MONITOR_THREAD_STACK_SIZE,
                zbus_monitor_thread, NULL, NULL, NULL,
                CONFIG_ZBUS_MONITOR_THREAD_PRIORITY, 0, 0);
