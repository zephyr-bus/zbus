#include <kernel.h>
#include <sys/printk.h>
#include "zbus.h"


int main()
{
    // printk("Size of __zt_channels = %u!\n", sizeof(*zt_channels_instance()));
    // struct version* v =
    //     ((struct version*) __zt_channels_lookup_table[zt_index_version]->channel);
    // printk("Sample version %d.%d.%d\n", v->major, v->minor, v->build);


    while (1) {
        k_sleep(K_FOREVER);
    }
    return (0);
}
