/**
 * @file      zbus.h
 * @brief     Header of
 * @date      Tue Dec 14 14:19:05 2021
 * @author    Rodrigo Peixoto
 * @copyright BSD-3-Clause
 *
 * This module
 */

#ifndef _ZBUS_H_
#define _ZBUS_H_

#include <string.h>
#include <zephyr.h>

#include "zbus_messages.h"

#if defined(CONFIG_ZBUS_ASSERTS)
#define ZB_ASSERT(cond)                                                                  \
    do {                                                                                 \
        if (!(cond)) {                                                                   \
            printk("Assertion failed %s:%d(%s): %s\n", __FILE__, __LINE__, __FUNCTION__, \
                   #cond);                                                               \
            k_oops();                                                                    \
        }                                                                                \
    } while (0)
#else
#define ZB_ASSERT(cond)
#endif

typedef enum {
#ifdef ZB_CHANNEL
#undef ZB_CHANNEL
#endif
#define ZB_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    zb_index_##name,
#include "zbus_channels.h"
    ZB_CHANNEL_COUNT
} __attribute__((packed)) zb_channel_index_t;


/**
 * @brief Check if _v value is equal to _c, otherwise _err will be
 * returned and a message will be sent to LOG.
 *
 * @param _v Value
 * @param _c Condition
 * @param _err Error code
 *
 */
#define ZB_CHECK_VAL(_p, _e, _err, ...) \
    if (_p == _e) {                     \
        LOG_INF(__VA_ARGS__);           \
        return _err;                    \
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
#define ZB_CHECK(_p, _err, ...) \
    if (_p) {                   \
        LOG_INF(__VA_ARGS__);   \
        return _err;            \
    }


/**
 * @def ZB_CHANNEL_INIT_DEFAULT_COMPLEX
 * This must be used to initialize structs with other structs or union inside itself.
 */
#define ZB_CHANNEL_INIT_DEFAULT_COMPLEX \
    {                                   \
        {                               \
            0                           \
        }                               \
    }


/**
 * @def ZB_CHANNEL_INIT_DEFAULT
 * This must be used to initialize structs with only scalar values inside.
 */
#define ZB_CHANNEL_INIT_DEFAULT \
    {                           \
        0                       \
    }

#define ZB_CHANNEL_INIT_VAL(init) init

#define ZB_INIT(val, ...)  \
    {                      \
        val, ##__VA_ARGS__ \
    }

struct metadata {
    struct {
        bool pend_callback;
        bool on_changed;
        bool read_only;
        bool source_serial_isc;
    } flag;
    uint16_t lookup_table_index;
    uint16_t channel_size;
    uint8_t *channel;
    struct k_sem *semaphore;
    struct k_msgq **subscribers;
};

#undef ZB_CHANNEL
#define ZB_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    struct metadata __zb_meta_##name;                                                    \
    type name;

struct zb_channels {
#include "zbus_channels.h"
};

struct zb_channels *__zb_channels_instance();

// /* To avoid error when not using LOG */
#if defined(CONFIG_ZBUS_LOG)
#define __ZB_LOG_DBG(...) LOG_DBG(__VA_ARGS__)
#else
#define __ZB_LOG_DBG(...)
#endif

#define ZB_CHANNEL_GET(chan) &__zb_channels_instance()->chan
#define ZB_CHANNEL_METADATA_GET(chan) \
    ((struct metadata *) &__zb_channels_instance()->__zb_meta_##chan)

#define zb_chan_pub(chan, value, timeout)                                               \
    ({                                                                                  \
        {                                                                               \
            __typeof__(__zb_channels_instance()->chan) chan##__aux__;                   \
            __typeof__(value) value##__aux__;                                           \
            (void) (&chan##__aux__ == &value##__aux__);                                 \
        }                                                                               \
        __ZB_LOG_DBG("[ZBUS] pub " #chan " at %s:%d", __FILE__, __LINE__);              \
        __zb_chan_pub(ZB_CHANNEL_METADATA_GET(chan), (uint8_t *) &value, sizeof(value), \
                      timeout);                                                         \
    })

#define zb_chan_pub_from_isr(chan, value) zb_chan_pub(chan, value, K_NO_WAIT)

int __zb_chan_pub(struct metadata *meta, uint8_t *data, size_t data_size,
                  k_timeout_t timeout);


#define zb_chan_read(chan, value, timeout)                                               \
    ({                                                                                   \
        {                                                                                \
            __typeof__(__zb_channels_instance()->chan) chan##__aux__;                    \
            __typeof__(value) value##__aux__;                                            \
            (void) (&chan##__aux__ == &value##__aux__);                                  \
        }                                                                                \
        __ZB_LOG_DBG("[ZBUS] read " #chan " at %s:%d", __FILE__, __LINE__);              \
        __zb_chan_read(ZB_CHANNEL_METADATA_GET(chan), (uint8_t *) &value, sizeof(value), \
                       timeout);                                                         \
    })

#define zb_chan_read_from_isr(chan, value) zb_chan_read(chan, value, K_NO_WAIT)

int __zb_chan_read(struct metadata *meta, uint8_t *data, size_t data_size,
                   k_timeout_t timeout);

#endif  // _ZBUS_H_
