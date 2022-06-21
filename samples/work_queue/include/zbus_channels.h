ZBUS_CHANNEL(version,              /* Name */
             false,                /* On changes only */
             true,                 /* Read only */
             struct version,       /* Message type */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(.major = 0, .minor = 1,
                           .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHANNEL(sensor_data,       /* Name */
             false,             /* On changes only */
             false,             /* Read only */
             struct sensor_msg, /* Message type */
             ZBUS_OBSERVERS(fast_handler1, fast_handler2, fast_handler3, delay_handler1,
                            delay_handler2, delay_handler3, thread_handler1,
                            thread_handler2, thread_handler3), /* observers */
             ZBUS_MSG_INIT(0)                                  /* Initial value {0} */
)
