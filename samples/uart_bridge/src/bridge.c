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

extern struct k_msgq _zbus_ext_msgq;

struct ct_uart_device bridge_uart = {DEVICE_DT_GET(DT_ALIAS(bridge_serial)),
                                     &_bridge_input_msgq, &_bridge_output_msgq};
void bridge_thread(void)
{
    zbus_channel_index_t idx        = 0;
    zbus_message_variant_t msg_data = {0};
    uint8_t tokens[]                = "$\r\n";

    if (ct_uart_open(&bridge_uart)) {
        return;
    }
    zbus_info_dump();
    ct_uart_write_str(&bridge_uart, "Hello world!\n");
    LOG_DBG("[Bridge] Started.");
    while (1) {
        if (!k_msgq_get(&_zbus_ext_msgq, &idx, K_FOREVER)) {
            struct zbus_channel *meta = zbus_chan_get_by_index(idx);

            LOG_DBG("[Bridge] send data %d",
                    zbus_chan_read(meta, (uint8_t *) &msg_data, meta->message_size,
                                   K_MSEC(500)));
            ct_uart_write_byte(&bridge_uart, tokens);
            ct_uart_write_byte(&bridge_uart, (uint8_t *) &idx);
            ct_uart_write(&bridge_uart, (uint8_t *) &msg_data, meta->message_size);
            ct_uart_write(&bridge_uart, tokens + 1, 2);
        }
    }
}

K_THREAD_DEFINE(bridge_thread_tid, 2048, bridge_thread, NULL, NULL, NULL, 1, 0, 500);
