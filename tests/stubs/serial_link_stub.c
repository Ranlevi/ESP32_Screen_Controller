#include "serial_link.h"

esp_err_t serial_link_init(const serial_link_cfg_t *cfg, const serial_link_stats_t **stats_out)
{
    (void)cfg;
    (void)stats_out;
    return ESP_OK;
}

esp_err_t serial_link_write(const uint8_t *data, size_t len)
{
    (void)data;
    (void)len;
    return ESP_OK;
}
