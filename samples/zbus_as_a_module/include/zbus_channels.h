ZBUS_CHANNEL(version,                        /* Name */
             false,                          /* Persistent */
             false,                          /* On changes only */
             true,                           /* Read only */
             struct version,                 /* Message type */
             ZBUS_OBSERVERS_EMPTY, /* Subscribers */
             ZBUS_MSG_INIT(.major = 0, .minor = 1,
                       .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHANNEL(chan_1b,                                 /* Name */
             false,                                   /* Persistent */
             false,                                   /* On changes only */
             false,                                   /* Read only */
             struct msg_1b,                           /* Message type */
             ZBUS_OBSERVERS(fast_handler1), /* Subscribers */
             ZBUS_MSG_INIT({0})                           /* Initial value {0} */
)

ZBUS_CHANNEL(chan_2b,                                 /* Name */
             false,                                   /* Persistent */
             false,                                   /* On changes only */
             false,                                   /* Read only */
             struct msg_2b,                           /* Message type */
             ZBUS_OBSERVERS(fast_handler1), /* Subscribers */
             ZBUS_MSG_INIT({0})                           /* Initial value {0} */
)

ZBUS_CHANNEL(chan_4b,                                 /* Name */
             false,                                   /* Persistent */
             false,                                   /* On changes only */
             false,                                   /* Read only */
             struct msg_4b,                           /* Message type */
             ZBUS_OBSERVERS(fast_handler1), /* Subscribers */
             ZBUS_MSG_INIT({0})                           /* Initial value {0} */
)

ZBUS_CHANNEL(chan_8b,                                 /* Name */
             false,                                   /* Persistent */
             false,                                   /* On changes only */
             false,                                   /* Read only */
             struct msg_8b,                           /* Message type */
             ZBUS_OBSERVERS(fast_handler1), /* Subscribers */
             ZBUS_MSG_INIT({0})                           /* Initial value {0} */
)

ZBUS_CHANNEL(chan_16b,                                /* Name */
             false,                                   /* Persistent */
             false,                                   /* On changes only */
             false,                                   /* Read only */
             struct msg_16b,                          /* Message type */
             ZBUS_OBSERVERS(fast_handler1), /* Subscribers */
             ZBUS_MSG_INIT({0})                           /* Initial value {0} */
)

ZBUS_CHANNEL(chan_32b,                                /* Name */
             false,                                   /* Persistent */
             false,                                   /* On changes only */
             false,                                   /* Read only */
             struct msg_32b,                          /* Message type */
             ZBUS_OBSERVERS(fast_handler1), /* Subscribers */
             ZBUS_MSG_INIT({0})                           /* Initial value {0} */
)

ZBUS_CHANNEL(chan_64b,                                /* Name */
             false,                                   /* Persistent */
             false,                                   /* On changes only */
             false,                                   /* Read only */
             struct msg_64b,                          /* Message type */
             ZBUS_OBSERVERS(fast_handler1), /* Subscribers */
             ZBUS_MSG_INIT({0})                           /* Initial value {0} */
)

ZBUS_CHANNEL(chan_128b,                               /* Name */
             false,                                   /* Persistent */
             false,                                   /* On changes only */
             false,                                   /* Read only */
             struct msg_128b,                         /* Message type */
             ZBUS_OBSERVERS(fast_handler1), /* Subscribers */
             ZBUS_MSG_INIT({0})                           /* Initial value {0} */
)

ZBUS_CHANNEL(chan_256b,                               /* Name */
             false,                                   /* Persistent */
             false,                                   /* On changes only */
             false,                                   /* Read only */
             struct msg_256b,                         /* Message type */
             ZBUS_OBSERVERS(fast_handler1), /* Subscribers */
             ZBUS_MSG_INIT({0})                           /* Initial value {0} */
)
