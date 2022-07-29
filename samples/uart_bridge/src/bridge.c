#include <kernel.h>
#include <string.h>
#include <sys/crc.h>
#include <sys/printk.h>
#include <zephyr.h>
#include "ct_uart.h"

#include "zbus.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

K_MSGQ_DEFINE(_bridge_input_msgq, sizeof(uint8_t), 256, 1);
K_MSGQ_DEFINE(_bridge_output_msgq, sizeof(uint8_t), 256, 1);

ZBUS_SUBSCRIBER_DECLARE(bridge, 4);

struct ct_uart_device bridge_uart = {DEVICE_DT_GET(DT_ALIAS(bridge_serial)),
                                     &_bridge_input_msgq, &_bridge_output_msgq};
void bridge_tx_thread(void)
{
    zbus_chan_idx_t idx         = 0;
    union zbus_msg_var msg_data = {0};
    uint8_t tokens[]            = "$\r\n";

    if (ct_uart_open(&bridge_uart)) {
        return;
    }
    zbus_info_dump();
    ct_uart_write_str(&bridge_uart, "Hello world!\n");
    LOG_DBG("[Bridge] Started.");
    while (1) {
        if (!k_msgq_get(bridge.queue, &idx, K_FOREVER)) {
            struct zbus_channel *chan = zbus_chan_get_by_index(idx);
            if (!zbus_chan_claim(chan, K_MSEC(500))) {
                bool generated_by_the_bridge = chan->user_data[0];
                chan->user_data[0] = (uint8_t) false;
                zbus_chan_finish(chan);
                /* true here means the package was published by the bridge and must be
                 * discarded */
                if (!generated_by_the_bridge) {
                    LOG_DBG("[Bridge] send data %d",
                            zbus_chan_read(chan, (uint8_t *) &msg_data, chan->message_size,
                                           K_MSEC(500)));
                    ct_uart_write_byte(&bridge_uart, tokens);
                    ct_uart_write_byte(&bridge_uart, (uint8_t *) &idx);
                    ct_uart_write(&bridge_uart, (uint8_t *) &msg_data, chan->message_size);
                    ct_uart_write(&bridge_uart, tokens + 1, 2);
                }
            }
        }
    }
}
K_THREAD_DEFINE(bridge_thread_tid, 2048, bridge_tx_thread, NULL, NULL, NULL, 1, 0, 500);
