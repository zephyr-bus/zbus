#ifndef _ZBUS_MESSAGES_H_
#define _ZBUS_MESSAGES_H_
#include <zephyr.h>

struct version {
    uint8_t major;
    uint8_t minor;
    uint16_t build;
};

struct sensor_data {
    int value;
};

struct action {
    bool status;
};

#endif  // _ZBUS_MESSAGES_H_
