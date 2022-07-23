ZBUS_CHAN_DEFINE(version,              /* Name */
             false,                /* On changes only */
             true,                 /* Read only */
             struct version,       /* Message type */
             NULL,                 /* Validator */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(.major = 0, .minor = 1,
                           .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZBUS_CHAN_DEFINE(sensor_data,          /* Name */
             true,                 /* On changes only */
             false,                /* Read only */
             struct sensor_data,   /* Message type */
             NULL,                 /* Validator */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(0)      /* Initial value {0} */
)

ZBUS_CHAN_DEFINE(start_measurement,          /* Name */
             false,                      /* On changes only */
             false,                      /* Read only */
             struct action,              /* Message type */
             NULL,                       /* Validator */
             ZBUS_OBSERVERS(peripheral), /* observers */
             ZBUS_MSG_INIT(false)        /* Initial value */
)

ZBUS_CHAN_DEFINE(finish,               /* Name */
             false,                /* On changes only */
             false,                /* Read only */
             struct action,        /* Message type */
             NULL,                 /* Validator */
             ZBUS_OBSERVERS_EMPTY, /* observers */
             ZBUS_MSG_INIT(false)  /* Initial value */
)
