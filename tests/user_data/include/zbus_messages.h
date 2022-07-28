#ifndef _ZBUS_MESSAGES_H_
#define _ZBUS_MESSAGES_H_
#include <stdint.h>
#include <zephyr.h>

struct version {
    uint8_t major;
    uint8_t minor;
    uint16_t build;
};

struct foo_msg {
    int a;
    int b;
};

#endif  // _ZBUS_MESSAGES_H_
