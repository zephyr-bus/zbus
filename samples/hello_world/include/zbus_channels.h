ZBUS_CHANNEL(version,              /* Name */
             false,                /* On changes only */
             true,                 /* Read only */
             struct version,       /* Message type */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(.major = 0, .minor = 1,
                           .build = 2) /* Initial value major 0, minor 1, build 1023 */
)


ZBUS_CHANNEL(acc_data,                                   /* Name */
             false,                                      /* On changes only */
             false,                                      /* Read only */
             struct acc_msg,                             /* Message type */
             ZBUS_OBSERVERS(my_listener, my_subscriber), /* observers */
             ZBUS_MSG_INIT(.x = 0, .y = 0, .z = 0)       /* Initial value {0} */
)
