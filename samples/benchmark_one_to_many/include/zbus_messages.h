#ifndef _ZBUS_MESSAGES_H_
#define _ZBUS_MESSAGES_H_
#include <zephyr.h>

// #ifndef BM_ONE_TO
// #define BM_ONE_TO 1LLU /*Must be defined by the compile command. See Makefile */
// #endif
// #ifndef BM_MESSAGE_SIZE
// #define BM_MESSAGE_SIZE 32 /*Must be defined by the compile command. See Makefile */
// #endif

struct bm_msg {
    uint8_t bytes[BM_MESSAGE_SIZE];
};

#endif  // _ZBUS_MESSAGES_H_
