#pragma once

#include "esp_err.h"
#include "serial_link.h"

typedef esp_err_t (*profiler_write_fn_t)(const uint8_t *data, size_t len);
typedef void      (*profiler_oled_fn_t)(const char *label, const char *value);

typedef struct {
    profiler_write_fn_t        write_fn;
    const serial_link_stats_t *serial_stats;
    profiler_oled_fn_t         oled_fn;
    const char                *fw_version;
    unsigned int               interval_ms;
    int                        task_stack_size;
    int                        task_priority;
} profiler_cfg_t;

#define PROFILER_DEFAULT_CONFIG() (profiler_cfg_t){ \
    .write_fn        = NULL,                         \
    .serial_stats    = NULL,                         \
    .oled_fn         = NULL,                         \
    .fw_version      = NULL,                         \
    .interval_ms     = 2000,                         \
    .task_stack_size = 3072,                         \
    .task_priority   = 1,                            \
}

esp_err_t profiler_init(const profiler_cfg_t *cfg);
void      profiler_set_oled_stat(const char *key);
