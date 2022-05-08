#ifndef _ZBUS_MESSAGES_H_
#define _ZBUS_MESSAGES_H_
#include <stdint.h>
#include <zephyr.h>

struct version {
    uint8_t major;
    uint8_t minor;
    uint16_t build;
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

struct s1 {
    int m;
    struct {
        int a, b, c;
    } field;
};

struct complex_msg {
    struct sensor_data act;
    struct sensor_data act2;
    struct s1 a;
    int action;
};


union union_msg {
    struct version v;
    struct net_pkt np;
    struct action a;
    struct s1 s;
    struct complex_msg cm;
};

#endif  // _ZBUS_MESSAGES_H_
