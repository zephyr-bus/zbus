#include "ct_uart.h"
#include <drivers/uart.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

static void uart_fifo_callback(const struct device *dev, void *user_data)
{
    struct ct_uart_device *uart = (struct ct_uart_device *) user_data;
    uint8_t data                = 0;
    if (!uart_irq_update(dev)) {
        LOG_ERR("It should be 1!");
        return;
    }

    if (uart_irq_tx_ready(dev)) {
        if (!k_msgq_get(uart->output_msgq, &data, K_NO_WAIT)) {
            uart_fifo_fill(dev, &data, sizeof(uint8_t));
        } else {
            uart_irq_tx_disable(dev);
        }
    }

    data = 0;

    if (uart_irq_rx_ready(dev)) {
        uart_fifo_read(dev, &data, sizeof(uint8_t));
        k_msgq_put(uart->input_msgq, &data, K_NO_WAIT);
    }
}

int ct_uart_open(struct ct_uart_device *uart)
{
    if (!device_is_ready(uart->dev)) {
        /* Not ready, do not use */
        return -ENODEV;
    }

    if (uart == NULL) {
        LOG_ERR("The UART device was not found.");
        return -EINVAL;
    }
    uart_irq_callback_user_data_set(uart->dev, uart_fifo_callback, uart);
    uart_irq_rx_enable(uart->dev);

    uart->ready = true;
    return 0;
}

int ct_uart_write(struct ct_uart_device *uart, uint8_t *data, size_t size)
{
    if (uart == NULL || uart->dev == NULL || uart->ready == false) {
        return -ENODEV;
    }
    int err            = 0;
    uint8_t *data_end  = data + size;
    uint8_t *data_iter = data;
    for (; data_iter < data_end; ++data_iter) {
        err = err || k_msgq_put(uart->output_msgq, data_iter, K_MSEC(10));
    }

    uart_irq_tx_enable(uart->dev);

    return err;
}

int ct_uart_write_str(struct ct_uart_device *uart, char *str)
{
    if (uart == NULL || uart->dev == NULL || uart->ready == false) {
        return -ENODEV;
    }
    int err         = 0;
    char *data_iter = str;
    for (; *data_iter != '\0'; ++data_iter) {
        err = err || k_msgq_put(uart->output_msgq, data_iter, K_MSEC(10));
    }

    uart_irq_tx_enable(uart->dev);

    return err;
}

int ct_uart_write_byte(struct ct_uart_device *uart, uint8_t *byte)
{
    if (uart == NULL || uart->dev == NULL) {
        return -ENODEV;
    }
    int err = k_msgq_put(uart->output_msgq, byte, K_MSEC(10));
    if (!err) {
        uart_irq_tx_enable(uart->dev);
    }
    return err;
}
