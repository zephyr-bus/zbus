/**
 * @file      zbus_channel_index.h
 * @brief     Header of
 * @date      Sun Nov 28 18:39:41 2021
 * @author    Rodrigo Peixoto
 * @copyright MIT
 *
 * This module
 */

#ifndef _ZBUS_PRIVATE_H_
#define _ZBUS_PRIVATE_H_
#include <zephyr.h>

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

#define ZT_CHANNEL_INIT_VAL_DEFAULT \
    {                               \
        0                           \
    }

#define ZT_CHANNEL_INIT_VAL(val, ...) \
    {                                 \
        val, ##__VA_ARGS__            \
    }
#define ZT_CHANNEL_NO_SUBSCRIBERS          \
    (struct k_msgq **) (struct k_msgq *[]) \
    {                                      \
        NULL                               \
    }
#define ZT_CHANNEL_SUBSCRIBERS_QUEUES(sub_ref, ...) \
    (struct k_msgq **) (struct k_msgq *[])          \
    {                                               \
        sub_ref, ##__VA_ARGS__, NULL                \
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


#endif  // _ZBUS_PRIVATE_H_
