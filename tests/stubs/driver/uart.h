#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

typedef int uart_port_t;

#define UART_NUM_0  0

//  Enums used in uart_config_t initialisation in serial_link.c
#define UART_DATA_8_BITS          3
#define UART_PARITY_DISABLE       0
#define UART_STOP_BITS_1          1
#define UART_HW_FLOWCTRL_DISABLE  0
#define UART_SCLK_DEFAULT         0

typedef struct {
    int baud_rate;
    int data_bits;
    int parity;
    int stop_bits;
    int flow_ctrl;
    int source_clk;
} uart_config_t;

esp_err_t uart_driver_install(uart_port_t port, int rx_buf, int tx_buf,
                               int queue_size, void *queue, int intr_flags);
esp_err_t uart_driver_delete(uart_port_t port);
esp_err_t uart_param_config(uart_port_t port, const uart_config_t *config);
int       uart_write_bytes(uart_port_t port, const void *src, size_t size);
int       uart_read_bytes(uart_port_t port, void *buf, uint32_t length,
                          TickType_t ticks_to_wait);
