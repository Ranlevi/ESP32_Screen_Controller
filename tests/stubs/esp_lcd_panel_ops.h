#pragma once

#include <stdbool.h>
#include "esp_err.h"

typedef void *esp_lcd_panel_handle_t;

esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t panel);
esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t panel);
esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t panel, bool on_off);
esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t panel);
esp_err_t esp_lcd_panel_draw_bitmap(esp_lcd_panel_handle_t panel,
                                     int x_start, int y_start,
                                     int x_end, int y_end,
                                     const void *color_data);
