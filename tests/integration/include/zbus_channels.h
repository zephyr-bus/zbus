ZB_CHANNEL(version,                      /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZB_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZB_INIT(.major = 0, .minor = 1,
                   .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZB_CHANNEL(sensor_data,                  /* Name */
           true,                         /* Persistent */
           true,                         /* On changes only */
           false,                        /* Read only */
           struct sensor_data,           /* Message type */
           ZB_CHANNEL_SUBSCRIBERS(core), /* Subscribers */
           ZB_INIT(0)                    /* Initial value {0} */
)

ZB_CHANNEL(net_pkt,                      /* Name */
           false,                        /* Persistent */
           true,                         /* On changes only */
           false,                        /* Read only */
           struct net_pkt,               /* Message type */
           ZB_CHANNEL_SUBSCRIBERS(net),  /* Subscribers */
           ZB_INIT(.y = false, .x = ' ') /* Initial value */
)

ZB_CHANNEL(start_measurement,                            /* Name */
           false,                                        /* Persistent */
           false,                                        /* On changes only */
           false,                                        /* Read only */
           struct action,                                /* Message type */
           ZB_CHANNEL_SUBSCRIBERS(peripheral, critical), /* Subscribers */
           ZB_INIT(false)                                /* Initial value */
)
