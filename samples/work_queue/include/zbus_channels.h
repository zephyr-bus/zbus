ZB_CHANNEL(version,                      /* Name */
           false,                        /* Persistent */
           false,                        /* On changes only */
           true,                         /* Read only */
           struct version,               /* Message type */
           ZB_CHANNEL_SUBSCRIBERS_EMPTY, /* Subscribers */
           ZB_INIT(.major = 0, .minor = 1,
                   .build = 1023) /* Initial value major 0, minor 1, build 1023 */
)

ZB_CHANNEL(sensor_data,       /* Name */
           false,             /* Persistent */
           false,             /* On changes only */
           false,             /* Read only */
           struct sensor_msg, /* Message type */
           ZB_CHANNEL_SUBSCRIBERS(fast_handler1, fast_handler2, fast_handler3,
                                  delay_handler1, delay_handler2, delay_handler3,
                                  thread_handler1, thread_handler2, thread_handler3), /* Subscribers */
           ZB_INIT(0)                              /* Initial value {0} */
)