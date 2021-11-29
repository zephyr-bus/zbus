#include <logging/log.h>
#include "zbus.h"
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

int main()
{
    struct version v = {0};
    zt_chan_read(version, v);

    LOG_INF("System version: %d.%d.%d", v.major, v.minor, v.build);
    while (1) {
        k_sleep(K_FOREVER);
    }
    return (0);
}
