#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_io.h"

esp_err_t esp_lcd_new_panel_ssd1306(esp_lcd_panel_io_handle_t io,
                                     const esp_lcd_panel_dev_config_t *panel_config,
                                     esp_lcd_panel_handle_t *ret_panel);
