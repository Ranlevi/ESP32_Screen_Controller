#pragma once

typedef struct {
    int bits_per_pixel;
    int reset_gpio_num;
    void *vendor_config;    // unused in stub; kept for API compatibility
} esp_lcd_panel_dev_config_t;
