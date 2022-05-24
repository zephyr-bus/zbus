#include "zbus_messages.h"
ZB_CHANNEL(version,                      /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZB_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZB_INIT(.major = 0, .minor = 1,
                   .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZB_CHANNEL(a,                            /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZB_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZB_INIT(0)                    /* Initial value major 0, minor 1, build 1023 */
)

ZB_CHANNEL(b,                            /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZB_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZB_INIT(0)                    /* Initial value major 0, minor 1, build 1023 */
)

ZB_CHANNEL(complex,                      /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct complex_msg,           /* Message type */
           ZB_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZB_INIT(.act = {0}, .act2 = {0},
                   .a      = {.m = 0, .field = {.a = 0, .b = 0, .c = 0}},
                   .action = 0) /* Initial value major 0, minor 1, build 1023 */
)

ZB_CHANNEL(union_complex,                /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           union union_msg,              /* Message type */
           ZB_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZB_INIT({0})                  /* Initial value major 0, minor 1, build 1023 */
)
