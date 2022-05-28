#ifndef _ZBUS_MESSAGES_H_
#define _ZBUS_MESSAGES_H_
#include <zephyr.h>

struct version {
    uint8_t major;
    uint8_t minor;
    uint16_t build;
};

struct msg_1b {
    uint8_t bytes[1];
};

struct msg_2b {
    uint8_t bytes[2];
};

struct msg_4b {
    uint8_t bytes[4];
};

struct msg_8b {
    uint8_t bytes[8];
};

struct msg_16b {
    uint8_t bytes[16];
};

struct msg_32b {
    uint8_t bytes[32];
};

struct msg_64b {
    uint8_t bytes[64];
};

struct msg_128b {
    uint8_t bytes[128];
};

struct msg_256b {
    uint8_t bytes[256];
};

#endif  // _ZBUS_MESSAGES_H_
