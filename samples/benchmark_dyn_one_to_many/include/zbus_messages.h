#ifndef _ZBUS_MESSAGES_H_
#define _ZBUS_MESSAGES_H_
#include <zephyr.h>

struct external_data_msg {
    void *reference;
    size_t size;
};

struct bm_msg {
    uint8_t bytes[BM_MESSAGE_SIZE];
};

#endif  // _ZBUS_MESSAGES_H_
