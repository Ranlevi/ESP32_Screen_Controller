#pragma once

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

//  Forward declaration — profiler.h only holds a pointer to this struct,
//  so it does not need the full definition. Callers that need to read the
//  struct fields (e.g. main.c) must include serial_link.h themselves.
typedef struct serial_link_stats_s serial_link_stats_t;

//  Pointer to a function.
//  The benefit: no need to include serial_link.h in the caller - any function
//  of that signature is valid, so you can swap serial comm with other transport.
//  Signature matches serial_link_write, so it can be passed directly.
typedef esp_err_t (*profiler_write_fn_t)(const uint8_t *data, size_t len);

//  Called by the profiler task each interval when an OLED stat is selected.
//  label: short human name (e.g. "Free Heap"), value: formatted string (e.g. "245.3 KB").
typedef void (*profiler_oled_fn_t)(const char *label, const char *value);

typedef struct {
    profiler_write_fn_t        write_fn;      // required — called with each JSON report
    const serial_link_stats_t *serial_stats;  // optional — NULL to omit bytes_rx/tx
    profiler_oled_fn_t         oled_fn;       // optional — NULL to disable OLED updates
    const char                *fw_version;    // optional — NULL to omit from JSON
    uint32_t                   interval_ms;   // report interval, must be > 0
    int                        task_stack_size;
    int                        task_priority;
} profiler_cfg_t;

//  Priority 1 is low-urgency compared to serial_link_rx which is 5.
//  Stack bumped to 3072 to accommodate vTaskGetRunTimeStats formatting.
#define PROFILER_DEFAULT_CONFIG() (profiler_cfg_t){  \
    .write_fn        = NULL,                          \
    .serial_stats    = NULL,                          \
    .oled_fn         = NULL,                          \
    .fw_version      = NULL,                          \
    .interval_ms     = 2000,                          \
    .task_stack_size = 3072,                          \
    .task_priority   = 1,                             \
}

//  Validates cfg, spawns the profiler task, and returns.
//  Idempotent: a second call with any cfg returns ESP_OK immediately.
esp_err_t profiler_init(const profiler_cfg_t *cfg);

//  Maximum length of a stat key string (e.g. "min_free_heap_b" = 15 chars).
//  Used by callers to size buffers that must hold an OLED: command.
#define PROFILER_OLED_KEY_MAX_LEN  15

//  Selects which stat key to display on the OLED each interval.
//  Pass an empty string or NULL to disable OLED updates.
//  Safe to call from any task at any time.
void profiler_set_oled_stat(const char *key);
