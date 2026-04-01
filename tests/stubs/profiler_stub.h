#pragma once

#include "esp_err.h"

typedef struct serial_link_stats_s serial_link_stats_t;

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

#define PROFILER_OLED_KEY_MAX_LEN  15

esp_err_t   profiler_init(const profiler_cfg_t *cfg);
void        profiler_set_oled_stat(const char *key);

//  Test accessors — call stub_profiler_reset() in setUp() before each test.
const char *stub_last_oled_key(void);
void        stub_profiler_reset(void);
