#pragma once
#include <device.h>
#include <inttypes.h>
#include <kernel.h>
#include <zephyr.h>

struct ct_uart_device {
    const struct device *dev;
    struct k_msgq *input_msgq;
    struct k_msgq *output_msgq;
    bool ready;
};

int ct_uart_open(struct ct_uart_device *uart);

int ct_uart_write(struct ct_uart_device *uart, uint8_t *data, size_t size);

int ct_uart_write_str(struct ct_uart_device *uart, char *str);

int ct_uart_write_byte(struct ct_uart_device *uart, uint8_t *byte);
