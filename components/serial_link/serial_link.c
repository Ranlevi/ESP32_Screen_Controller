#include "serial_link.h"

#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

static const char *TAG = "serial_link";

static serial_link_cfg_t s_cfg;
static serial_link_stats_t s_stats;
static SemaphoreHandle_t s_tx_lock;
static bool s_initialized;

static void serial_link_rx_task(void *arg)
{
    (void)arg;

    uint8_t rx_buffer[128];
    const TickType_t read_timeout_ticks = pdMS_TO_TICKS(s_cfg.read_timeout_ms);

    while (true) {
        const int read_len = uart_read_bytes(
            s_cfg.uart_port,
            rx_buffer,
            sizeof(rx_buffer),
            read_timeout_ticks);

        if (read_len < 0) {
            ESP_LOGW(TAG, "uart_read_bytes failed: %d", read_len);
            continue;
        }

        if (read_len == 0) {
            continue;
        }

        s_stats.bytes_rx += (uint32_t)read_len;
        serial_link_write(rx_buffer, (size_t)read_len);
    }
}

esp_err_t serial_link_init(const serial_link_cfg_t *cfg, const serial_link_stats_t **stats_out)
{
    if (cfg == NULL || stats_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_initialized) {
        *stats_out = &s_stats;
        return ESP_OK;
    }

    if (cfg->rx_buffer_size <= 0 || cfg->tx_buffer_size <= 0 || cfg->task_stack_size <= 0 || cfg->task_priority <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    s_cfg = *cfg;
    memset(&s_stats, 0, sizeof(s_stats));

    const uart_config_t uart_cfg = {
        .baud_rate = s_cfg.baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    esp_err_t err = uart_driver_install(
        s_cfg.uart_port,
        s_cfg.rx_buffer_size,
        s_cfg.tx_buffer_size,
        0,
        NULL,
        0);
    if (err != ESP_OK) {
        return err;
    }

    err = uart_param_config(s_cfg.uart_port, &uart_cfg);
    if (err != ESP_OK) {
        uart_driver_delete(s_cfg.uart_port);
        return err;
    }

    s_tx_lock = xSemaphoreCreateMutex();
    if (s_tx_lock == NULL) {
        uart_driver_delete(s_cfg.uart_port);
        return ESP_ERR_NO_MEM;
    }

    BaseType_t task_ok = xTaskCreate(
        serial_link_rx_task,
        "serial_link_rx",
        (uint32_t)s_cfg.task_stack_size,
        NULL,
        (UBaseType_t)s_cfg.task_priority,
        NULL);

    if (task_ok != pdPASS) {
        vSemaphoreDelete(s_tx_lock);
        s_tx_lock = NULL;
        uart_driver_delete(s_cfg.uart_port);
        return ESP_ERR_NO_MEM;
    }

    s_initialized = true;
    *stats_out = &s_stats;
    return ESP_OK;
}

esp_err_t serial_link_write(const uint8_t *data, size_t len)
{
    if (!s_initialized || data == NULL || len == 0U) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_tx_lock == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (xSemaphoreTake(s_tx_lock, portMAX_DELAY) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    size_t remaining = len;
    const uint8_t *cursor = data;
    while (remaining > 0U) {
        const int written = uart_write_bytes(s_cfg.uart_port, cursor, remaining);
        if (written <= 0) {
            xSemaphoreGive(s_tx_lock);
            return ESP_FAIL;
        }

        remaining -= (size_t)written;
        cursor += written;
        s_stats.bytes_tx += (uint32_t)written;
    }

    xSemaphoreGive(s_tx_lock);
    return ESP_OK;
}
