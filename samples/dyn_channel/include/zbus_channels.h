
ZBUS_CHANNEL(pkt_channel,                      /* Name */
             false,                            /* Persistent */
             false,                            /* On changes only */
             false,                            /* Read only */
             struct external_data_msg,         /* Message type */
             ZBUS_CHANNEL_SUBSCRIBERS(filter), /* Subscribers */
             ZBUS_INIT(0)                      /* Initial value {0} */
)

ZBUS_CHANNEL(version,                                      /* Name */
             false,                                        /* Persistent */
             false,                                        /* On changes only */
             true,                                         /* Read only */
             struct version_msg,                           /* Message type */
             ZBUS_CHANNEL_SUBSCRIBERS_EMPTY,               /* Subscribers */
             ZBUS_INIT(.major = 0, .minor = 1, .build = 1) /* Initial value {0} */
)

ZBUS_CHANNEL(data_ready,                         /* Name */
             false,                              /* Persistent */
             false,                              /* On changes only */
             false,                              /* Read only */
             struct ack_msg,                     /* Message type */
             ZBUS_CHANNEL_SUBSCRIBERS(consumer), /* Subscribers */
             ZBUS_INIT(0)                        /* Initial value {0} */
)

ZBUS_CHANNEL(ack,                                /* Name */
             false,                              /* Persistent */
             false,                              /* On changes only */
             false,                              /* Read only */
             struct ack_msg,                     /* Message type */
             ZBUS_CHANNEL_SUBSCRIBERS(producer), /* Subscribers */
             ZBUS_INIT(0)                        /* Initial value {0} */
)
