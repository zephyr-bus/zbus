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

void proxy_callback(zbus_chan_idx_t idx);

ZBUS_LISTENER_DECLARE(proxy, proxy_callback);

const uint8_t tokens[] = "$*";

void proxy_callback(zbus_chan_idx_t idx)
{
    LOG_DBG("[Mock Proxy callback] started (channel %d)", idx);
    struct zbus_channel *chan = zbus_chan_get_by_index(idx);
    if (!zbus_chan_claim(chan, K_NO_WAIT)) {
        bool *generated_by_the_bridge = (bool *) chan->user_data;
        if (*generated_by_the_bridge) {
            *generated_by_the_bridge = false;
            LOG_DBG("[Mock Proxy callback] filtered loopback event (channel %d)", idx);
        } else {
            ct_uart_write_byte(&mock_proxy_uart, (uint8_t *) &tokens[0]);
            ct_uart_write_byte(&mock_proxy_uart, (uint8_t *) &idx);
            ct_uart_write(&mock_proxy_uart, chan->message, chan->message_size);
            ct_uart_write(&mock_proxy_uart, (uint8_t *) &tokens[1], 1);
            LOG_DBG("[Mock Proxy callback] sending message to host (channel %d)", idx);
        }
        zbus_chan_finish(chan);
    }
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

    uint8_t byte        = 0;
    zbus_chan_idx_t idx = ZBUS_CHAN_COUNT;
    while (1) {
        if (!k_msgq_get(&_mock_proxy_input_msgq, &byte, K_FOREVER)) {
            if (byte == '*') {
                if (rx_buf->len <= 1) {
                    LOG_DBG("[Mock Proxy RX] Discard invalid sequence");
                    /* '*' indicates the end of a sentence. Sometimes it is necessary to
                     * flush more than on ensure sending it from the python script. The
                     * code must discard when there is no other data at the buffer. */
                } else {
                    if ('$' == net_buf_simple_pull_u8(rx_buf)) {
                        LOG_DBG("[Mock Proxy RX] Found sentence");
                        idx = net_buf_simple_pull_u8(rx_buf);
                        if (idx < ZBUS_CHAN_COUNT) {
                            struct zbus_channel *chan = zbus_chan_get_by_index(idx);
                            if (!zbus_chan_claim(chan, K_NO_WAIT)) {
                                memcpy(
                                    chan->message,
                                    net_buf_simple_pull_mem(rx_buf, chan->message_size),
                                    chan->message_size);
                                bool *generated_by_the_bridge = (bool *) chan->user_data;
                                *generated_by_the_bridge      = true;
                                zbus_chan_finish(chan);
                                LOG_DBG("[Mock Proxy RX] Publishing channel %d", idx);
                                zbus_chan_notify(chan, K_MSEC(500));
                            }
                        }
                    }
                    net_buf_simple_init(rx_buf, 0);
                }
            } else {
                net_buf_simple_add_u8(rx_buf, byte);
            }
        }
    }
}

K_THREAD_DEFINE(mock_proxy_rx_thread_tid, 2048, mock_proxy_rx_thread, NULL, NULL, NULL, 1,
                0, 1500);
