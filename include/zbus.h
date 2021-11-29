#ifndef _ZBUS_H_
#define _ZBUS_H_

#include <string.h>
#include <zephyr.h>

#include "zbus_private.h"
#include "zeta_messages.h"


// struct zt_channels __zt_channels = {0};
struct zt_channels *zt_channels_instance();

int __zt_sem_take(void *semaphore);

int __zt_sem_give(void *semaphore);


#define ZT_CHANNEL_GET(chan) &zt_channels_instance()->chan
#define ZT_CHANNEL_METADATA_GET(chan) \
    ((struct metadata *) &zt_channels_instance()->__zt_meta_##chan)

#define zt_chan_pub(chan, value)                                                         \
    ({                                                                                   \
        {                                                                                \
            __typeof__(zt_channels_instance()->chan) chan##__aux__;                      \
            __typeof__(value) value##__aux__;                                            \
            (void) (&chan##__aux__ == &value##__aux__);                                  \
        }                                                                                \
        __zt_chan_pub(ZT_CHANNEL_METADATA_GET(chan), (uint8_t *) &value, sizeof(value)); \
    })

int __zt_chan_pub(struct metadata *meta, uint8_t *data, size_t data_size);


#define zt_chan_read(chan, value)                                         \
    ({                                                                    \
        {                                                                 \
            __typeof__(zt_channels_instance()->chan) chan##__aux__;       \
            __typeof__(value) value##__aux__;                             \
            (void) (&chan##__aux__ == &value##__aux__);                   \
        }                                                                 \
        __zt_chan_read(ZT_CHANNEL_METADATA_GET(chan), (uint8_t *) &value, \
                       sizeof(value));                                    \
    })

int __zt_chan_read(struct metadata *meta, uint8_t *data, size_t data_size);

#endif  // _ZBUS_H_
