#pragma once
#include <stdbool.h>

struct queue {
    int a[20];
};

struct queue sub1;
struct queue sub2;
struct queue sub3;

struct version {
    uint8_t major;
    uint8_t minor;
    uint16_t build;
};

struct machine {
    int a;
    int b;
};

struct oven {
    char x;
    bool y;
};
