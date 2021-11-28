ZT_CHANNEL(version,                        /* Name */
           false,                          /* Persistent */
           false,                          /* On change only */
           true,                           /* Read only */
           struct version,                 /* Message type */
           ZT_CHANNEL_NO_SUBSCRIBERS,      /* Subscribers */
           ZT_CHANNEL_INIT_VAL(0, 1, 1023) /* Initial value {0} */
)

ZT_CHANNEL(machine,                                     /* Name */
           true,                                        /* Persistent */
           false,                                       /* On change only */
           false,                                       /* Read only */
           struct machine,                              /* Message type */
           ZT_CHANNEL_SUBSCRIBERS_QUEUES(&sub1, &sub2), /* Subscribers */
           ZT_CHANNEL_INIT_VAL_DEFAULT                  /* Initial value {0} */
)

ZT_CHANNEL(oven,                           /* Name */
           false,                          /* Persistent */
           true,                           /* On change only */
           false,                          /* Read only */
           struct oven,                    /* Message type */
           ZT_CHANNEL_NO_SUBSCRIBERS,      /* Subscribers */
           ZT_CHANNEL_INIT_VAL('B', false) /* Initial value */
)
