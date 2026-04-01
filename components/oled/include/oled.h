#pragma once

#include "esp_err.h"

//  Initializes the I2C master bus and the SSD1306 panel.
//  Must be called once before any other oled_* function.
esp_err_t oled_init(void);

//  Renders a null-terminated string starting at character cell (col, row).
//  col is in pixels (0-127), row is in pixels (0-56 for an 8px-tall font; last full character row).
//  Silently clips text that extends beyond the display boundary.
//  Blits into the framebuffer AND flushes to the display in one call.
esp_err_t oled_show_text(int col, int row, const char *text);

//  Blits a string into the framebuffer only — does NOT flush to the display.
//  Call oled_flush() once after all blit operations to send in one transfer.
esp_err_t oled_blit_text(int col, int row, const char *text);

//  Sends the current framebuffer to the display over I2C.
esp_err_t oled_flush(void);

//  Clears the entire display to black (blits and flushes).
esp_err_t oled_clear(void);
