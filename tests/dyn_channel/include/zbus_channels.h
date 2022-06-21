ZBUS_CHANNEL(version,              /* Name */
             false,                /* On changes only */
             true,                 /* Read only */
             struct version,       /* Message type */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(.major = 0, .minor = 1,
                           .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHANNEL(sensor_data,          /* Name */
             true,                 /* On changes only */
             false,                /* Read only */
             struct sensor_data,   /* Message type */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(0)      /* Initial value {0} */
)

ZBUS_CHANNEL(dyn_chan_no_subs,         /* Name */
             false,                    /* On changes only */
             false,                    /* Read only */
             struct external_data_msg, /* Message type */
             ZBUS_OBSERVERS_EMPTY,     /* observers */
             ZBUS_MSG_INIT(0)          /* Initial value {0} */
)

ZBUS_CHANNEL(dyn_chan,                 /* Name */
             true,                     /* On changes only */
             false,                    /* Read only */
             struct external_data_msg, /* Message type */
             ZBUS_OBSERVERS(s1),       /* observers */
             ZBUS_MSG_INIT(0)          /* Initial value {0} */
)
