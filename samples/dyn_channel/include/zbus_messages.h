#ifndef _ZBUS_MESSAGES_H_
#define _ZBUS_MESSAGES_H_
#include <zephyr.h>

struct version_msg {
    uint8_t major;
    uint8_t minor;
    uint16_t build;
};

struct ack_msg {
    uint8_t value;
};

#endif  // _ZBUS_MESSAGES_H_
