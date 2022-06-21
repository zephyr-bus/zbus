
ZBUS_CHANNEL(pkt_channel,              /* Name */
             false,                    /* On changes only */
             false,                    /* Read only */
             struct external_data_msg, /* Message type */
             ZBUS_OBSERVERS(filter),   /* observers */
             ZBUS_MSG_INIT(0)          /* Initial value {0} */
)

ZBUS_CHANNEL(version,                                          /* Name */
             false,                                            /* On changes only */
             true,                                             /* Read only */
             struct version_msg,                               /* Message type */
             ZBUS_OBSERVERS_EMPTY,                             /* observers */
             ZBUS_MSG_INIT(.major = 0, .minor = 1, .build = 1) /* Initial value {0} */
)

ZBUS_CHANNEL(data_ready,               /* Name */
             false,                    /* On changes only */
             false,                    /* Read only */
             struct ack_msg,           /* Message type */
             ZBUS_OBSERVERS(consumer), /* observers */
             ZBUS_MSG_INIT(0)          /* Initial value {0} */
)

ZBUS_CHANNEL(ack,                      /* Name */
             false,                    /* On changes only */
             false,                    /* Read only */
             struct ack_msg,           /* Message type */
             ZBUS_OBSERVERS(producer), /* observers */
             ZBUS_MSG_INIT(0)          /* Initial value {0} */
)
