#include <logging/log.h>
#include "zbus.h"
LOG_MODULE_DECLARE(peripheral, CONFIG_ZBUS_LOG_LEVEL);

void urgent_callback()
{
    printk(" *** CRITICAL ***\n");
}

ZB_SUBSCRIBER_REGISTER_CALLBACK(critical, urgent_callback);
