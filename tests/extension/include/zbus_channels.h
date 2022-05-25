ZBUS_CHANNEL(version,                      /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZBUS_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZBUS_INIT(.major = 0, .minor = 1,
                   .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHANNEL(sensor_data,                  /* Name */
           true,                         /* Persistent */
           true,                         /* On changes only */
           false,                        /* Read only */
           struct sensor_data,           /* Message type */
           ZBUS_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZBUS_INIT(0)                    /* Initial value {0} */
)

ZBUS_CHANNEL(start_measurement,                  /* Name */
           false,                              /* Persistent */
           false,                              /* On changes only */
           false,                              /* Read only */
           struct action,                      /* Message type */
           ZBUS_CHANNEL_SUBSCRIBERS(peripheral), /* Subscribers */
           ZBUS_INIT(false)                      /* Initial value */
)

ZBUS_CHANNEL(finish,                       /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           false,                        /* Read only */
           struct action,                /* Message type */
           ZBUS_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZBUS_INIT(false)                /* Initial value */
)
