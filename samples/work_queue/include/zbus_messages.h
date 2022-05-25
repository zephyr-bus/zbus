#ifndef _ZBUS_MESSAGES_H_
#define _ZBUS_MESSAGES_H_
#include <zephyr.h>

struct version {
    uint8_t major;
    uint8_t minor;
    uint16_t build;
};

struct sensor_msg {
    uint32_t temp;
    uint32_t press;
    uint32_t humidity;
};

#endif  // _ZBUS_MESSAGES_H_
