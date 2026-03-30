#include "profiler.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

static const char *TAG = "profiler";

static profiler_cfg_t s_cfg;
static bool           s_initialized = false;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

//  Copies src into dst as a JSON-safe string (no surrounding quotes).
//  Escapes \n → \n, \t → \t, \r → (dropped), " → \", \ → \\.
//  Returns the number of characters written (excluding null terminator).
#ifdef PROFILER_TEST_ACCESS
int escape_for_json(const char *src, char *dst, size_t dst_size);
#endif

#ifdef PROFILER_TEST_ACCESS
int
#else
static int
#endif
escape_for_json(const char *src, char *dst, size_t dst_size)
{
    size_t di = 0;
    for (size_t si = 0; src[si] != '\0' && di + 2 < dst_size; si++) {
        char c = src[si];
        if      (c == '\n') { dst[di++] = '\\'; dst[di++] = 'n';  }
        else if (c == '\t') { dst[di++] = '\\'; dst[di++] = 't';  }
        else if (c == '"')  { dst[di++] = '\\'; dst[di++] = '"';  }
        else if (c == '\\') { dst[di++] = '\\'; dst[di++] = '\\'; }
        else if (c == '\r') { /* drop */ }
        else                { dst[di++] = c; }
    }
    dst[di] = '\0';
    return (int)di;
}

static void send(const char *buf, int n)
{
    if (n <= 0 || (size_t)n >= 1024 /* generous upper bound */) {
        return;
    }
    esp_err_t err = s_cfg.write_fn((const uint8_t *)buf, (size_t)n);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "write_fn failed: %s", esp_err_to_name(err));
    }
}

// ---------------------------------------------------------------------------
// Task
// ---------------------------------------------------------------------------

static void profiler_task(void *arg)
{
    (void)arg;

    //Normally, local variables live in the stack. Here we have three large
    //buffers which would consume two thirds of the stack. So we declare them
    //as static, which moves them out of the stack to the BSS segment of the
    //RAM, and leaves the stack nearly empty.
    static char sys_buf[256];
    static char task_raw[512];
    static char task_escaped[768];
    static char task_buf[832];

    for (;;) {
        //The SysTick HW timer fires an interrupt every 1ms. Each int increments
        //FreeRTOS's tick counter. When the tick counter reaches the given wakeup
        //count, the RTOS moves profiler_task from blocked to Ready, and then Running.
        vTaskDelay(pdMS_TO_TICKS(s_cfg.interval_ms));

        // --- collect scalars ---
        uint32_t uptime_s      = (uint32_t)(esp_timer_get_time() / 1000000LL);
        uint32_t free_heap     = esp_get_free_heap_size();
        uint32_t min_free_heap = esp_get_minimum_free_heap_size();
        uint32_t task_count    = (uint32_t)uxTaskGetNumberOfTasks();
        int      reset_reason  = (int)esp_reset_reason();
        uint32_t bytes_rx      = s_cfg.serial_stats ? s_cfg.serial_stats->bytes_rx : 0;
        uint32_t bytes_tx      = s_cfg.serial_stats ? s_cfg.serial_stats->bytes_tx : 0;

        // --- system stats JSON ---
        int n = snprintf(sys_buf, sizeof(sys_buf),
            "{\"uptime_s\":%lu,\"free_heap_b\":%lu,\"min_free_heap_b\":%lu,"
            "\"task_count\":%lu,\"reset_reason\":%d,"
            "\"cpu_freq_mhz\":%d,\"idf_version\":\"%s\","
            "\"bytes_rx\":%lu,\"bytes_tx\":%lu}\r\n",
            (unsigned long)uptime_s,
            (unsigned long)free_heap,
            (unsigned long)min_free_heap,
            (unsigned long)task_count,
            reset_reason,
            CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
            IDF_VER,
            (unsigned long)bytes_rx,
            (unsigned long)bytes_tx);

        send(sys_buf, n);

        // --- task runtime stats JSON ---
        //  vTaskGetRunTimeStats requires CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS=y.
        //  It writes a plain-text table (tab-separated, newline-delimited) into the
        //  buffer. We escape it and wrap it in a JSON string field so the web app
        //  can parse it without a custom framing protocol.
#ifdef CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS
        vTaskGetRunTimeStats(task_raw);
        escape_for_json(task_raw, task_escaped, sizeof(task_escaped));
        int m = snprintf(task_buf, sizeof(task_buf),
                         "{\"task_runtime\":\"%s\"}\r\n", task_escaped);
        send(task_buf, m);
#endif
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

esp_err_t profiler_init(const profiler_cfg_t *cfg)
{
    if (cfg == NULL || cfg->write_fn == NULL ||
        cfg->interval_ms == 0 ||
        cfg->task_stack_size <= 0 || cfg->task_priority <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_initialized) {
        return ESP_OK;
    }

    s_cfg = *cfg;

    BaseType_t ok = xTaskCreate(
        profiler_task,
        "profiler",
        (uint32_t)s_cfg.task_stack_size,
        NULL,
        (UBaseType_t)s_cfg.task_priority,
        NULL);

    //  xTaskCreate returns pdPASS if the task was created successfully.
    if (ok != pdPASS) {
        return ESP_ERR_NO_MEM;
    }

    s_initialized = true;
    ESP_LOGI(TAG, "started (interval %lu ms)", (unsigned long)s_cfg.interval_ms);
    return ESP_OK;
}
