#pragma once

#include <stdint.h>
#include <stdbool.h>

#define LCD_FRAMEBUFFER_SIZE 1024   // 128 * 64 / 8

//  Returns a pointer to the last framebuffer passed to esp_lcd_panel_draw_bitmap.
const uint8_t *stub_lcd_bitmap(void);

//  Returns true if esp_lcd_panel_draw_bitmap has been called since the last reset.
bool stub_lcd_draw_called(void);

//  Resets all captured state (call in setUp).
void stub_lcd_reset(void);
