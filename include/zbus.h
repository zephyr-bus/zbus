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


typedef enum {
#ifdef ZT_CHANNEL
#undef ZT_CHANNEL
#endif
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    zt_index_##name,
#include "zbus_channels.def"
    ZT_CHANNEL_COUNT
} __attribute__((packed)) zt_channel_index_t;


/**
 * @brief Check if _v value is equal to _c, otherwise _err will be
 * returned and a message will be sent to LOG.
 *
 * @param _v Value
 * @param _c Condition
 * @param _err Error code
 *
 */
#define ZT_CHECK_VAL(_p, _e, _err, ...) \
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
#define ZT_CHECK(_p, _err, ...) \
    if (_p) {                   \
        LOG_INF(__VA_ARGS__);   \
        return _err;            \
    }


/**
 * @def ZT_CHANNEL_INIT_DEFAULT_COMPLEX
 * This must be used to initialize structs with other structs or union inside itself.
 */
#define ZT_CHANNEL_INIT_DEFAULT_COMPLEX \
    {                                   \
        {                               \
            0                           \
        }                               \
    }


/**
 * @def ZT_CHANNEL_INIT_DEFAULT
 * This must be used to initialize structs with only scalar values inside.
 */
#define ZT_CHANNEL_INIT_DEFAULT \
    {                           \
        0                       \
    }

#define ZT_CHANNEL_INIT_VAL(init) init

#define ZT_INIT(val, ...)  \
    {                      \
        val, ##__VA_ARGS__ \
    }

struct metadata {
    char *name;
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

#undef ZT_CHANNEL
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    struct metadata __zt_meta_##name;                                                    \
    type name;

struct zt_channels {
#include "zbus_channels.def"
};

struct zt_channels *__zt_channels_instance();

#define ZT_CHANNEL_GET(chan) &__zt_channels_instance()->chan
#define ZT_CHANNEL_METADATA_GET(chan) \
    ((struct metadata *) &__zt_channels_instance()->__zt_meta_##chan)

#define zt_chan_pub(chan, value)                                                         \
    ({                                                                                   \
        {                                                                                \
            __typeof__(__zt_channels_instance()->chan) chan##__aux__;                    \
            __typeof__(value) value##__aux__;                                            \
            (void) (&chan##__aux__ == &value##__aux__);                                  \
        }                                                                                \
        __zt_chan_pub(ZT_CHANNEL_METADATA_GET(chan), (uint8_t *) &value, sizeof(value)); \
    })

int __zt_chan_pub(struct metadata *meta, uint8_t *data, size_t data_size);


#define zt_chan_read(chan, value)                                         \
    ({                                                                    \
        {                                                                 \
            __typeof__(__zt_channels_instance()->chan) chan##__aux__;     \
            __typeof__(value) value##__aux__;                             \
            (void) (&chan##__aux__ == &value##__aux__);                   \
        }                                                                 \
        __zt_chan_read(ZT_CHANNEL_METADATA_GET(chan), (uint8_t *) &value, \
                       sizeof(value));                                    \
    })

int __zt_chan_read(struct metadata *meta, uint8_t *data, size_t data_size);

#endif  // _ZBUS_H_
