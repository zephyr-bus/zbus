ZBUS_CHANNEL(bm_channel,    /* Name */
             false,         /* On changes only */
             false,         /* Read only */
             struct bm_msg, /* Message type */
             NULL,          /* Validator */
             ZBUS_OBSERVERS(s1

#if (BM_ONE_TO >= 2LLU)
                            ,
                            s2
#if (BM_ONE_TO > 2LLU)
                            ,
                            s3, s4
#if (BM_ONE_TO > 4LLU)
                            ,
                            s5, s6, s7, s8
#if (BM_ONE_TO > 8LLU)
                            ,
                            s9, s10, s11, s12, s13, s14, s15, s16
#endif
#endif
#endif
#endif
                            ),  /* observers */
             ZBUS_MSG_INIT({0}) /* Initial value {0} */
)
