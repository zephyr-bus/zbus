ZBUS_CHAN_DEFINE(
    version,              /* Name */
    false,                /* On changes only */
    true,                 /* Read only */
    struct version,       /* Message type */
    NULL,                 /* Validator */
    ZBUS_OBSERVERS_EMPTY, /* observers */
    ZBUS_MSG_INIT(.major = 0, .minor = 1,
           .build = 2) /* Initial value major 0, minor 1, build 1023 */
)
ZBUS_CHAN_DEFINE(
    regular,                    /* Name */
    false,                /* On changes only */
    false,                 /* Read only */
    struct foo_msg,       /* Message type */
    NULL,                 /* Validator */
    ZBUS_OBSERVERS(foo_listener, foo_subscriber), /* observers */
    ZBUS_MSG_INIT(0)      /* Initial value major 0, minor 1, build 1023 */
)