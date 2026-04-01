#include "serial_link.h"

#include <stdbool.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

static const char *TAG = "serial_link"; //Used in ESP_LOGW

static serial_link_cfg_t    s_cfg;
static serial_link_stats_t  s_stats;
static SemaphoreHandle_t    s_tx_lock;
static bool                 s_initialized;

/*
    RX Mechanism:
    The ESP32's UART hardware has an RX FIFO, which is filled with incoming bytes.
    When the FIFO is full, an interrupt (set up by uart_driver_install) fires
    and calls an ISR that drains the FIFO into an internal ring buffer in RAM.
    uart_read_bytes() waits for a FreeRTOS semaphore (sent by the ISR) 
    that signals that new data is in the ring. The function wakes up, copy the 
    new data to the rx_buffer and returns.
    Also, if the readout timer expires, uart_read_bytes() will read the ring buffer
    anyway to check if new data arrives.
*/
static void serial_link_rx_task(void *arg)
{
    (void)arg;

    uint8_t rx_buffer[128];

    //Convert read_timeout_ms (given in millisecs) to schedular ticks.
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

        //  bytes_rx is written only from this task, and read by the profiler task.
        //  On the ESP32, a naturally-aligned uint32_t read/write is a single
        //  instruction and is therefore atomic — no mutex needed. If this field
        //  is ever widened to uint64_t this assumption must be revisited.
        s_stats.bytes_rx += (uint32_t)read_len;
        if (s_cfg.echo) {
            serial_link_write(rx_buffer, (size_t)read_len);
        }

        //rx_callback is a pointer to on_rx, defined in main.c
        //on_rx runs on serial_link_rx task's stack, because it is called
        //directly by serial_link_rx: there's no new task created or queue 
        //involved - just a normal function call. So the CPU pushes a stack
        //frame into the currently active frame. 
        if (s_cfg.rx_callback != NULL) {
            s_cfg.rx_callback(rx_buffer, (size_t)read_len);
        }
    }
}

//Configuration of the UART controller in the ESP32.
esp_err_t serial_link_init(const serial_link_cfg_t *cfg, const serial_link_stats_t **stats_out)
{
    if (cfg == NULL || stats_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    //idempotency guard: prevents execution a 2nd time (s_initialzed
    //will be false the first time the function is called).
    if (s_initialized) {
        *stats_out = &s_stats;
        return ESP_OK;
    }

    //Verify non-zero or negative values, which are configured
    //in the struct.
    if (cfg->rx_buffer_size <= 0 || cfg->tx_buffer_size <= 0 || cfg->task_stack_size <= 0 || cfg->task_priority <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    s_cfg = *cfg;
    memset(&s_stats, 0, sizeof(s_stats));

    const uart_config_t uart_cfg = {
        .baud_rate  = s_cfg.baud_rate,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    //The following flow (install, set comm params, etc.) is described in the UART component documentation.

    //Allocate required internal resources for the UART driver.
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

    //UART pins are not set explicitly, since the default
    //config is correct for this board.

    err = uart_param_config(s_cfg.uart_port, &uart_cfg);
    if (err != ESP_OK) {
        uart_driver_delete(s_cfg.uart_port);
        return err;
    }

    //Create the mutex that protects the UART output 
    //(See more below)
    s_tx_lock = xSemaphoreCreateMutex();
    if (s_tx_lock == NULL) {
        uart_driver_delete(s_cfg.uart_port);
        return ESP_ERR_NO_MEM;
    }

    //Create the RX task
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

/*  Mutex guard:
    serial_link_write() can be called from multiple tasks.
    If the function is called before all the bytes were
    written, the output would interleave the bytes from
    the two tasks, corrupting both messages.
    So serial_link_write() acquires the lock (xSemaphoreTake) before touching
    the UART, blocking the caller until it is released (xSemaphoreGive).

    portMAX_DELAY is intentional: a finite timeout would cause the caller to
    silently drop data, which is worse than blocking. A stuck mutex is a bug;
    the correct outcome is that the task watchdog fires and produces a
    diagnostic reboot rather than hiding the problem behind dropped messages.
*/
esp_err_t serial_link_write(const uint8_t *data, size_t len)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL || len == 0U) {
        return ESP_ERR_INVALID_ARG;
    }

    xSemaphoreTake(s_tx_lock, portMAX_DELAY);

    size_t remaining        = len;
    const uint8_t *cursor   = data;

    while (remaining > 0U) {
        const int written = uart_write_bytes(s_cfg.uart_port, cursor, remaining);
        if (written <= 0) {
            xSemaphoreGive(s_tx_lock);
            return ESP_FAIL;
        }

        remaining -= (size_t)written;
        cursor += written;
        //  bytes_tx is written here under s_tx_lock, and read by the profiler
        //  task without a lock. Safe for the same reason as bytes_rx above:
        //  naturally-aligned uint32_t access is atomic on the ESP32.
        s_stats.bytes_tx += (uint32_t)written;
    }

    xSemaphoreGive(s_tx_lock);
    return ESP_OK;
}
