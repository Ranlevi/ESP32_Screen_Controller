#include "profiler_stub.h"

esp_err_t profiler_init(const profiler_cfg_t *cfg)
{
    (void)cfg;
    return ESP_OK;
}

void profiler_set_oled_stat(const char *key)
{
    (void)key;
}
