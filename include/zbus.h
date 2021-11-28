#ifndef _ZBUS_H_
#define _ZBUS_H_

#include <string.h>
#include <zephyr.h>

#include "zbus_private.h"
#include "zeta_messages.h"


#undef ZT_CHANNEL
#define ZT_CHANNEL(name, persistant, on_changed, read_only, type, subscribers, init_val) \
    struct {                                                                             \
        struct metadata __zt_meta_##name;                                                \
        type name;                                                                       \
    };
struct zt_channels {
#include "zbus_channels.def"
} __zt_channels = {0};

int __zt_sem_take(void *semaphore);

int __zt_sem_give(void *semaphore);


#define ZT_CHANNEL_GET(chan) &__zt_channels.chan
#define ZT_CHANNEL_METADATA_GET(chan) \
    ((struct metadata *) &__zt_channels.__zt_meta_##chan)

#define zt_chan_pub(chan, value)                                                         \
    ({                                                                                   \
        {                                                                                \
            __typeof__(__zt_channels.chan) chan##__aux__;                                \
            __typeof__(value) value##__aux__;                                            \
            (void) (&chan##__aux__ == &value##__aux__);                                  \
        }                                                                                \
        __zt_chan_pub(ZT_CHANNEL_METADATA_GET(chan), (uint8_t *) &value, sizeof(value)); \
    })

int __zt_chan_pub(struct metadata *meta, uint8_t *data, size_t data_size);


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

int __zt_chan_read(struct metadata *meta, uint8_t *data, size_t data_size);

#endif  // _ZBUS_H_
