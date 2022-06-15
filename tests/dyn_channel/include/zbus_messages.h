#ifndef _ZBUS_MESSAGES_H_
#define _ZBUS_MESSAGES_H_
#include <zephyr.h>

struct version {
    uint8_t major;
    uint8_t minor;
    uint16_t build;
};

struct external_data_msg {
    void *reference;
    size_t size;
};

struct sensor_data {
    int a;
    int b;
};

struct net_pkt {
    char x;
    bool y;
};

struct action {
    bool status;
};

#endif  // _ZBUS_MESSAGES_H_
