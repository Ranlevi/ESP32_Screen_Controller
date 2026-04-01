#include "profiler_stub.h"

#include <string.h>

static char s_last_oled_key[32] = "";

esp_err_t profiler_init(const profiler_cfg_t *cfg)
{
    (void)cfg;
    return ESP_OK;
}

void profiler_set_oled_stat(const char *key)
{
    if (key == NULL) {
        s_last_oled_key[0] = '\0';
    } else {
        strncpy(s_last_oled_key, key, sizeof(s_last_oled_key) - 1);
        s_last_oled_key[sizeof(s_last_oled_key) - 1] = '\0';
    }
}

const char *stub_last_oled_key(void)  { return s_last_oled_key; }
void        stub_profiler_reset(void) { s_last_oled_key[0] = '\0'; }
