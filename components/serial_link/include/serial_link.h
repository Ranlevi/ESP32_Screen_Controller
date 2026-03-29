#pragma once

#include <stddef.h>
#include <stdint.h>

#include "driver/uart.h"
#include "esp_err.h"

typedef struct {
    uint32_t bytes_rx;
    uint32_t bytes_tx;
    uint32_t rx_overflow_events;
} serial_link_stats_t;

typedef struct {
    uart_port_t uart_port;
    int baud_rate;
    int rx_buffer_size;
    int tx_buffer_size;
    int task_stack_size;
    int task_priority;
    int read_timeout_ms;
} serial_link_cfg_t;

#define SERIAL_LINK_DEFAULT_CONFIG()        \
    (serial_link_cfg_t) {                   \
        .uart_port = UART_NUM_0,            \
        .baud_rate = 115200,                \
        .rx_buffer_size = 1024,             \
        .tx_buffer_size = 1024,             \
        .task_stack_size = 3072,            \
        .task_priority = 5,                 \
        .read_timeout_ms = 100,             \
    }

esp_err_t serial_link_init(const serial_link_cfg_t *cfg, const serial_link_stats_t **stats_out);
esp_err_t serial_link_write(const uint8_t *data, size_t len);
