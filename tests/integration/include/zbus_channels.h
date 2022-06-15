ZBUS_CHANNEL(version,                      /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZBUS_OBSERVERS_EMPTY, /* Subscribers */
           ZBUS_MSG_INIT(.major = 0, .minor = 1,
                   .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHANNEL(sensor_data,                  /* Name */
           true,                         /* Persistent */
           true,                         /* On changes only */
           false,                        /* Read only */
           struct sensor_data,           /* Message type */
           ZBUS_OBSERVERS(core), /* Subscribers */
           ZBUS_MSG_INIT(0)                    /* Initial value {0} */
)

ZBUS_CHANNEL(net_pkt,                      /* Name */
           false,                        /* Persistent */
           true,                         /* On changes only */
           false,                        /* Read only */
           struct net_pkt,               /* Message type */
           ZBUS_OBSERVERS(net),  /* Subscribers */
           ZBUS_MSG_INIT(.y = false, .x = ' ') /* Initial value */
)

ZBUS_CHANNEL(start_measurement,                            /* Name */
           false,                                        /* Persistent */
           false,                                        /* On changes only */
           false,                                        /* Read only */
           struct action,                                /* Message type */
           ZBUS_OBSERVERS(peripheral, critical), /* Subscribers */
           ZBUS_MSG_INIT(false)                                /* Initial value */
)
