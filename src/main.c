#include <kernel.h>
#include <sys/printk.h>
#include "zbus.h"
#include "zeta_messages.h"


int main()
{
    struct version v = {0};
    zt_chan_read(version, v);

    printk("System version: %d.%d.%d\n", v.major, v.minor, v.build);
    while (1) {
        k_sleep(K_FOREVER);
    }
    return (0);
}
