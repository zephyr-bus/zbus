#include <kernel.h>
#include <net/buf.h>
#include <stdint.h>
#include <string.h>
#include <sys/crc.h>
#include <sys/printk.h>
#include <zephyr.h>
#include "ct_uart.h"
#include "zbus.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

K_MSGQ_DEFINE(_mock_proxy_input_msgq, sizeof(uint8_t), 256, 1);
K_MSGQ_DEFINE(_mock_proxy_output_msgq, sizeof(uint8_t), 256, 1);


struct ct_uart_device mock_proxy_uart = {DEVICE_DT_GET(DT_NODELABEL(uart1)),
                                         &_mock_proxy_input_msgq,
                                         &_mock_proxy_output_msgq};

void proxy_callback(zbus_channel_index_t idx);
ZBUS_LISTENER_DECLARE(proxy, proxy_callback);


const uint8_t tokens[] = "$*";
void proxy_callback(zbus_channel_index_t idx)
{
    LOG_DBG("[Mock Proxy callback] started");
    zbus_message_variant_t msg_data = {0};
    ZBUS_CHAN_READ_BY_INDEX(idx, msg_data, K_MSEC(200));
    ct_uart_write_byte(&mock_proxy_uart, (uint8_t *) &tokens[0]);
    ct_uart_write_byte(&mock_proxy_uart, (uint8_t *) &idx);
    ct_uart_write(&mock_proxy_uart, (uint8_t *) &msg_data,
                  zbus_channel_get_by_index(idx)->message_size);
    ct_uart_write(&mock_proxy_uart, (uint8_t *) &tokens[1], 1);
    LOG_DBG("[Mock Proxy callback] sending sensor data to host");
}

void main(void)
{
    if (ct_uart_open(&mock_proxy_uart)) {
        return;
    }
    zbus_info_dump();
    LOG_DBG("[Mock Proxy] Started.");
}


void mock_proxy_rx_thread(void)
{
    LOG_DBG("[Mock Proxy RX] Started.");

    struct net_buf_simple *rx_buf = NET_BUF_SIMPLE(64);
    net_buf_simple_init(rx_buf, 0);

    uint8_t byte                   = 0;
    zbus_channel_index_t idx       = ZBUS_CHANNEL_COUNT;
    zbus_message_variant_t variant = {0};
    while (1) {
        if (!k_msgq_get(&_mock_proxy_input_msgq, &byte, K_FOREVER)) {
            if (byte == '*') {
                if ('$' == net_buf_simple_pull_u8(rx_buf)) {
                    idx = net_buf_simple_pull_u8(rx_buf);
                    if (idx < ZBUS_CHANNEL_COUNT) {
                        struct zbus_channel *meta = zbus_channel_get_by_index(idx);
                        memcpy(&variant,
                               net_buf_simple_pull_mem(rx_buf, meta->message_size),
                               meta->message_size);
                        ZBUS_CHAN_PUB_BY_INDEX(idx, variant, K_MSEC(200));
                    }
                }
                net_buf_simple_init(rx_buf, 0);
            } else {
                net_buf_simple_add_u8(rx_buf, byte);
            }
        }
    }
}

K_THREAD_DEFINE(mock_proxy_rx_thread_tid, 2048, mock_proxy_rx_thread, NULL, NULL, NULL, 1,
                0, 1500);
