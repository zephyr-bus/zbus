/**
 * @file      zbus.h
 * @brief     Header of
 * @date      Tue Dec 14 14:19:05 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 */

#ifndef _ZBUS_H_
#define _ZBUS_H_

#include <string.h>
#include <zephyr.h>

#include "zbus_messages.h"

#if defined(CONFIG_ZBUS_ASSERTS)
#define ZBUS_ASSERT(cond)                                                                \
    do {                                                                                 \
        if (!(cond)) {                                                                   \
            printk("Assertion failed %s:%d(%s): %s\n", __FILE__, __LINE__, __FUNCTION__, \
                   #cond);                                                               \
            k_oops();                                                                    \
        }                                                                                \
    } while (0)
#else
#define ZBUS_ASSERT(cond)
#endif

struct zbus_dyn_message {
    void *ref;
    size_t size;
};

#ifndef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val)
#endif

typedef enum __attribute__((packed)) {
#ifdef ZBUS_CHANNEL
#undef ZBUS_CHANNEL
#endif
#define ZBUS_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, \
                     init_val)                                                   \
    zbus_index_##name,
#include "zbus_channels.h"
    ZBUS_CHANNEL_COUNT
} zbus_channel_index_t;


/**
 * @brief Check if _v value is equal to _c, otherwise _err will be
 * returned and a message will be sent to LOG.
 *
 * @param _v Value
 * @param _c Condition
 * @param _err Error code
 *
 */
#define ZBUS_CHECK_VAL(_p, _e, _err, ...) \
    if (_p == _e) {                       \
        LOG_INF(__VA_ARGS__);             \
        return _err;                      \
    }

/**
 * @brief Check if _v is true, otherwise _err will be returned and a
 * message will be sent to LOG.
 *
 * @param _v Value
 * @param _err Error code
 *
 * @return
 */
#define ZBUS_CHECK(_p, _err, ...) \
    if (_p) {                     \
        LOG_INF(__VA_ARGS__);     \
        return _err;              \
    }


/**
 * @def ZBUS_CHANNEL_INIT_DEFAULT_COMPLEX
 * This must be used to initialize structs with other structs or union inside itself.
 */
#define ZBUS_CHANNEL_INIT_DEFAULT_COMPLEX \
    {                                     \
        {                                 \
            0                             \
        }                                 \
    }


/**
 * @def ZBUS_CHANNEL_INIT_DEFAULT
 * This must be used to initialize structs with only scalar values inside.
 */
#define ZBUS_CHANNEL_INIT_DEFAULT \
    {                             \
        0                         \
    }

#define ZBUS_CHANNEL_INIT_VAL(init) init

#define ZBUS_MSG_INIT(val, ...) \
    {                       \
        val, ##__VA_ARGS__  \
    }

#define ZBUS_SUBSCRIBER_REGISTER(name, queue_size)                        \
    K_MSGQ_DEFINE(name##_queue, sizeof(zbus_channel_index_t), queue_size, \
                  sizeof(zbus_channel_index_t));                          \
    struct zbus_subscriber name = {                                       \
        .enabled  = true,                                                 \
        .queue    = &name##_queue,                                        \
        .callback = NULL,                                                 \
    }

#define ZBUS_LISTENER_REGISTER(name, cb) \
    struct zbus_subscriber name = {      \
        .enabled  = true,                \
        .queue    = NULL,                \
        .callback = cb,                  \
    }


struct zbus_subscriber {
    bool enabled;
    struct k_msgq *queue;
    void (*callback)(zbus_channel_index_t idx);
};

void zbus_subscriber_set_enable(struct zbus_subscriber *sub, bool enabled);

struct zbus_channel {
    struct {
        uint8_t pend_callback : 1;
        uint8_t on_changed : 1;
        uint8_t read_only : 1;
        uint8_t from_ext : 1;
    } flag;
    uint16_t lookup_table_index;
    uint16_t message_size;
    uint8_t *message;
    struct k_sem *semaphore;
    struct zbus_subscriber **subscribers;
};

#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, \
                     init_val)                                                   \
    struct zbus_channel __zbus_chan_##name;                                      \
    type name;

struct zbus_channels {
#include "zbus_channels.h"
};

#undef ZBUS_CHANNEL
#define ZBUS_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, \
                     init_val)                                                   \
    type name;

typedef union {
#include "zbus_channels.h"
} zbus_message_variant_t;

struct zbus_channels *__zbus_channels_instance();
struct zbus_channel *zbus_channel_get_by_index(zbus_channel_index_t idx);

// /* To avoid error when not using LOG */
#if defined(CONFIG_ZBUS_LOG)
#define __ZBUS_LOG_DBG(...) LOG_DBG(__VA_ARGS__)
#else
#define __ZBUS_LOG_DBG(...)
#endif

#define ZBUS_CHANNEL_GET(chan) \
    ((struct zbus_channel *) &__zbus_channels_instance()->__zbus_chan_##chan)

void zbus_info_dump(void);

#define ZBUS_CHAN_PUB(chan, value, timeout)                                              \
    ({                                                                                   \
        {                                                                                \
            __typeof__(__zbus_channels_instance()->chan) chan##__aux__;                  \
            __typeof__(value) value##__aux__;                                            \
            (void) (&chan##__aux__ == &value##__aux__);                                  \
        }                                                                                \
        __ZBUS_LOG_DBG("[ZBUS] %spub " #chan " at %s:%d", (k_is_in_isr() ? "ISR " : ""), \
                       __FILE__, __LINE__);                                              \
        __zbus_chan_pub(ZBUS_CHANNEL_GET(chan), (uint8_t *) &value, sizeof(value),       \
                        timeout, false);                                                 \
    })

#define zbus_chan_pub_by_index(idx, value, timeout)                                    \
    ({                                                                                 \
        __ZBUS_LOG_DBG("[ZBUS] %spub %d at %s:%d", (k_is_in_isr() ? "ISR " : ""), idx, \
                       __FILE__, __LINE__);                                            \
        __zbus_chan_pub(zbus_channel_get_by_index(idx), (uint8_t *) &value,            \
                        zbus_channel_get_by_index(idx)->message_size, timeout, false); \
    })

int __zbus_chan_pub(struct zbus_channel *meta, uint8_t *msg, size_t msg_size,
                    k_timeout_t timeout, bool from_ext);


#define ZBUS_CHAN_READ(chan, value, timeout)                                      \
    ({                                                                            \
        {                                                                         \
            __typeof__(__zbus_channels_instance()->chan) chan##__aux__;           \
            __typeof__(value) value##__aux__;                                     \
            (void) (&chan##__aux__ == &value##__aux__);                           \
        }                                                                         \
        __ZBUS_LOG_DBG("[ZBUS] %sread " #chan " at %s:%d",                        \
                       (k_is_in_isr() ? "ISR " : ""), __FILE__, __LINE__);        \
        zbus_chan_read(ZBUS_CHANNEL_GET(chan), (uint8_t *) &value, sizeof(value), \
                       timeout);                                                  \
    })

#define ZBUS_CHAN_READ_BY_INDEX(idx, value, timeout)                                    \
    ({                                                                                  \
        __ZBUS_LOG_DBG("[ZBUS] %sread %d at %s:%d", (k_is_in_isr() ? "ISR " : ""), idx, \
                       __FILE__, __LINE__);                                             \
        zbus_chan_read(zbus_channel_get_by_index(idx), (uint8_t *) &value,              \
                       zbus_channel_get_by_index(idx)->message_size, timeout);          \
    })

int zbus_chan_read(struct zbus_channel *meta, uint8_t *msg, size_t msg_size,
                   k_timeout_t timeout);

int zbus_chan_claim(struct zbus_channel *meta, void **chan_msg, k_timeout_t timeout);

void zbus_chan_finish(struct zbus_channel *meta, k_timeout_t timeout);

int zbus_chan_notify(struct zbus_channel *meta, k_timeout_t timeout);


#endif  // _ZBUS_H_
