#pragma once

#include "esp_err.h"

//  Initializes the I2C master bus and the SSD1306 panel.
//  Must be called once before any other oled_* function.
esp_err_t oled_init(void);

//  Renders a null-terminated string starting at character cell (col, row).
//  col is in pixels (0-127), row is in pixel rows (0-55 for 8px-tall font).
//  Silently clips text that extends beyond the display boundary.
esp_err_t oled_show_text(int col, int row, const char *text);

//  Clears the entire display to black.
esp_err_t oled_clear(void);
