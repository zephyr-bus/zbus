ZBUS_CHAN_DEFINE(pkt_channel,              /* Name */
                 false,                    /* On changes only */
                 false,                    /* Read only */
                 struct external_data_msg, /* Message type */
                 NULL,                     /* Validator */
                 ZBUS_OBSERVERS(filter),   /* observers */
                 ZBUS_MSG_INIT(0)          /* Initial value {0} */
)

ZBUS_CHAN_DEFINE(version,                                          /* Name */
                 false,                                            /* On changes only */
                 true,                                             /* Read only */
                 struct version_msg,                               /* Message type */
                 NULL,                                             /* Validator */
                 ZBUS_OBSERVERS_EMPTY,                             /* observers */
                 ZBUS_MSG_INIT(.major = 0, .minor = 1, .build = 1) /* Initial value {0} */
)

ZBUS_CHAN_DEFINE(data_ready,               /* Name */
                 false,                    /* On changes only */
                 false,                    /* Read only */
                 struct ack_msg,           /* Message type */
                 NULL,                     /* Validator */
                 ZBUS_OBSERVERS(consumer), /* observers */
                 ZBUS_MSG_INIT(0)          /* Initial value {0} */
)

ZBUS_CHAN_DEFINE(ack,                      /* Name */
                 false,                    /* On changes only */
                 false,                    /* Read only */
                 struct ack_msg,           /* Message type */
                 NULL,                     /* Validator */
                 ZBUS_OBSERVERS(producer), /* observers */
                 ZBUS_MSG_INIT(0)          /* Initial value {0} */
)
