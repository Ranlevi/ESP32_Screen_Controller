#include "driver/uart.h"

esp_err_t uart_driver_install(uart_port_t port, int rx_buf, int tx_buf,
                               int queue_size, void *queue, int intr_flags)
{
    (void)port; (void)rx_buf; (void)tx_buf;
    (void)queue_size; (void)queue; (void)intr_flags;
    return ESP_OK;
}

esp_err_t uart_driver_delete(uart_port_t port)
{
    (void)port;
    return ESP_OK;
}

esp_err_t uart_param_config(uart_port_t port, const uart_config_t *config)
{
    (void)port;
    (void)config;
    return ESP_OK;
}

int uart_write_bytes(uart_port_t port, const void *src, size_t size)
{
    (void)port;
    (void)src;
    return (int)size;   // report all bytes written
}

int uart_read_bytes(uart_port_t port, void *buf, uint32_t length,
                    TickType_t ticks_to_wait)
{
    (void)port; (void)buf; (void)length; (void)ticks_to_wait;
    return 0;   // no data available
}
