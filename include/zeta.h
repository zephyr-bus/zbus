#pragma once
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "zeta_messages.h"

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
#define ZT_CHANNEL_GET(chan) &zeta_channels.chan
#define ZT_CHANNEL_METADATA_GET(chan) \
    ((struct metadata *) &zeta_channels.__zt_meta_##chan)

#define ZT_CHANNEL_INIT_VAL_DEFAULT \
    {                               \
        0                           \
    }

#define ZT_CHANNEL_INIT_VAL(val, ...) \
    {                                 \
        val, ##__VA_ARGS__            \
    }
#define ZT_CHANNEL_NO_SUBSCRIBERS \
    (void **) (void *[])          \
    {                             \
        NULL                      \
    }
#define ZT_CHANNEL_SUBSCRIBERS_QUEUES(sub_ref, ...) \
    (void **) (void *[])                            \
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
    void *semaphore;
    void **subscribers;
};


enum {
#ifdef ZT_CHANNEL
#undef ZT_CHANNEL
#endif
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    __index_##name,
#include "zeta_channels.h"
    ZT_CHANNEL_COUNT
} lookup_table_index;


#undef ZT_CHANNEL
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    struct {                                                                             \
        struct metadata __zt_meta_##name;                                                \
        type name;                                                                       \
    };
struct __zt_channels {
#include "zeta_channels.h"
} zeta_channels = {
#undef ZT_CHANNEL
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    .__zt_meta_##name = {#name,        __index_##name,                                   \
                         persistant,   on_changed,                                       \
                         read_only,    0,                                                \
                         sizeof(type), (uint8_t *) &zeta_channels.name,                  \
                         subscribers},                                                   \
    .name             = init_val,
#include "zeta_channels.h"
};
/*! The result of the macro usage is:
ZT_CHANNELS_DEFINE(ZT_CHANNEL(machine, struct machine_msg), ZT_CHANNEL(oven, struct
oven_msg));

Result:
struct zeta_channels {
    struct {
        struct metadata __zt_meta_machine;
        struct machine_msg machine;
    };
    struct {
        struct metadata __zt_meta_oven;
        struct oven_msg oven;
    };
} zeta_channels = {.__zt_meta_machine = {"MACHINE", false, true, 10, sizeof(struct
 machine_msg), (uint8_t *) &zeta_channels.machine}};
*/

struct metadata *__zt_channels_lookup_table[] = {
#undef ZT_CHANNEL
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    &zeta_channels.__zt_meta_##name,
#include "zeta_channels.h"
};

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
            typeof(zeta_channels.chan) chan##__aux__;                                    \
            typeof(value) value##__aux__;                                                \
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
    // ZT_CHECK(k_sem_take(meta->sem, K_MSEC(200)) != 0, -EBUSY,
    //          "Could not publish the channel. Channel is busy");
    if (meta->flag.on_changed) {  // CHANGE
        if (memcmp(meta->channel, data, data_size) == 0) {
            /* This data is not different from the channel's. No changes here. */
            meta->flag.pend_callback = false;
            goto cleanup;
        }
    }
    // #if defined(CONFIG_ZETA_SERIAL_IPC)
    // 	if (k_current_get() == zt_serial_ipc_thread()) {
    // 		meta->flag.field.source_serial_isc = 1;
    // 	}
    // #endif
    memcpy(meta->channel, data, channel_size);
    meta->flag.pend_callback = true;
    // int error = k_msgq_put(&zt_channels_changed_msgq, (uint8_t *) &id, K_MSEC(500));
    // if (error != 0) {
    // 	LOG_INF("[Channel #%d] Error sending channels change message to ZT "
    // 			"thread!",
    // 			id);
    // }
cleanup:
    // k_sem_give(meta->semaphore);
    return 0;
}


#define zt_chan_read(chan, value)                                         \
    ({                                                                    \
        {                                                                 \
            typeof(zeta_channels.chan) chan##__aux__;                     \
            typeof(value) value##__aux__;                                 \
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
    // ZT_CHECK(k_sem_take(meta->sem, K_MSEC(200)) != 0, -EBUSY,
    //          "Could not read the channel. Channel is busy");
    memcpy(data, meta->channel, meta->channel_size);
    return 0;
}
