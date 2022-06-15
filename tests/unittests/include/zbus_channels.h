#include "zbus_messages.h"
ZBUS_CHANNEL(version,                      /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZBUS_OBSERVERS_EMPTY, /* Subscribers */
           ZBUS_MSG_INIT(.major = 0, .minor = 1,
                   .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHANNEL(a,                            /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZBUS_OBSERVERS_EMPTY, /* Subscribers */
           ZBUS_MSG_INIT(0)                    /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHANNEL(b,                            /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZBUS_OBSERVERS_EMPTY, /* Subscribers */
           ZBUS_MSG_INIT(0)                    /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHANNEL(complex,                      /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct complex_msg,           /* Message type */
           ZBUS_OBSERVERS_EMPTY, /* Subscribers */
           ZBUS_MSG_INIT(.act = {0}, .act2 = {0},
                   .a      = {.m = 0, .field = {.a = 0, .b = 0, .c = 0}},
                   .action = 0) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHANNEL(union_complex,                /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           union union_msg,              /* Message type */
           ZBUS_OBSERVERS_EMPTY, /* Subscribers */
           ZBUS_MSG_INIT({0})                  /* Initial value major 0, minor 1, build 1023 */
)
