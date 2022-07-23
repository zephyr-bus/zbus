#include "zbus_messages.h"
ZBUS_CHAN_DEFINE(version,              /* Name */
             false,                /* On changes only */
             true,                 /* Read only */
             struct version,       /* Message type */
             NULL,                 /* Validator */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(.major = 0, .minor = 1,
                           .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHAN_DEFINE(a,                    /* Name */
             false,                /* On changes only */
             true,                 /* Read only */
             struct version,       /* Message type */
             NULL,                 /* Validator */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(0)      /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHAN_DEFINE(b,                    /* Name */
             false,                /* On changes only */
             true,                 /* Read only */
             struct version,       /* Message type */
             NULL,                 /* Validator */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(0)      /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHAN_DEFINE(complex,              /* Name */
             false,                /* On changes only */
             true,                 /* Read only */
             struct complex_msg,   /* Message type */
             NULL,                 /* Validator */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(.act = {0}, .act2 = {0},
                           .a      = {.m = 0, .field = {.a = 0, .b = 0, .c = 0}},
                           .action = 0) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHAN_DEFINE(union_complex,        /* Name */
             false,                /* On changes only */
             true,                 /* Read only */
             union union_msg,      /* Message type */
             NULL,                 /* Validator */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT({0})    /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHAN_DEFINE(hard_channel,         /* Name */
             false,                /* On changes only */
             false,                /* Read only */
             struct hard_msg,      /* Message type */
             hard_msg_validator,   /* Validator */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(0)    /* Initial value major 0, minor 1, build 1023 */
)
