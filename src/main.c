#include "zeta.h"

int main()
{
    printf("Size of zeta_channels = %lu!\n", sizeof(zeta_channels));
    struct version* v =
        ((struct version*) __zt_channels_lookup_table[__index_version]->channel);
    printf("Sample version %d.%d.%d\n", v->major, v->minor, v->build);

    printf("Before machine.a = %d\n", zeta_channels.machine.a);
    zeta_channels.machine.a = 10;
    printf("After machine.a = %d\n",
           ((struct machine*) __zt_channels_lookup_table[__index_machine]->channel)->a);

    struct machine t = {20, 30};
    zt_chan_pub(machine, t);
    printf("After machine.a = %d\n",
           ((struct machine*) __zt_channels_lookup_table[__index_machine]->channel)->a);
    struct machine t2 = {10, 10};
    zt_chan_read(machine, t2);
    printf("After machine.a = %d\n", t2.a);
    struct oven o1 = {'A', true};
    struct oven* before =
        ((struct oven*) __zt_channels_lookup_table[__index_oven]->channel);
    printf("Before oven.x = %c, oven.y = %s\n", before->x, before->y ? "true" : "false");
    zt_chan_pub(oven, o1);
    struct oven o2 = {0};
    zt_chan_read(oven, o2);
    printf("After oven.x = %c, oven.y = %s\n", o2.x, o2.y ? "true" : "false");
    return (0);
}
