#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_stub.h"

#include <string.h>
#include <stdbool.h>

// ---------------------------------------------------------------------------
// Captured state
// ---------------------------------------------------------------------------

static uint8_t s_last_bitmap[LCD_FRAMEBUFFER_SIZE];
static bool    s_draw_called = false;

//  Non-NULL sentinel used as a fake panel handle.
static int s_panel_sentinel;

// ---------------------------------------------------------------------------
// I2C stubs
// ---------------------------------------------------------------------------

esp_err_t i2c_new_master_bus(const i2c_master_bus_config_t *config,
                              i2c_master_bus_handle_t *ret_handle)
{
    (void)config;
    *ret_handle = (i2c_master_bus_handle_t)&s_panel_sentinel;
    return ESP_OK;
}

esp_err_t i2c_del_master_bus(i2c_master_bus_handle_t handle)
{
    (void)handle;
    return ESP_OK;
}

// ---------------------------------------------------------------------------
// Panel IO stubs
// ---------------------------------------------------------------------------

esp_err_t esp_lcd_new_panel_io_i2c(i2c_master_bus_handle_t bus,
                                    const esp_lcd_panel_io_i2c_config_t *io_config,
                                    esp_lcd_panel_io_handle_t *ret_io)
{
    (void)bus;
    (void)io_config;
    *ret_io = (esp_lcd_panel_io_handle_t)&s_panel_sentinel;
    return ESP_OK;
}

esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io)
{
    (void)io;
    return ESP_OK;
}

// ---------------------------------------------------------------------------
// Panel driver stubs
// ---------------------------------------------------------------------------

esp_err_t esp_lcd_new_panel_ssd1306(esp_lcd_panel_io_handle_t io,
                                     const esp_lcd_panel_dev_config_t *panel_config,
                                     esp_lcd_panel_handle_t *ret_panel)
{
    (void)io;
    (void)panel_config;
    *ret_panel = (esp_lcd_panel_handle_t)&s_panel_sentinel;
    return ESP_OK;
}

esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t panel)   { (void)panel; return ESP_OK; }
esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t panel)    { (void)panel; return ESP_OK; }
esp_err_t esp_lcd_panel_del(esp_lcd_panel_handle_t panel)     { (void)panel; return ESP_OK; }

esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t panel, bool on_off)
{
    (void)panel;
    (void)on_off;
    return ESP_OK;
}

esp_err_t esp_lcd_panel_draw_bitmap(esp_lcd_panel_handle_t panel,
                                     int x_start, int y_start,
                                     int x_end, int y_end,
                                     const void *color_data)
{
    (void)panel; (void)x_start; (void)y_start; (void)x_end; (void)y_end;
    memcpy(s_last_bitmap, color_data, LCD_FRAMEBUFFER_SIZE);
    s_draw_called = true;
    return ESP_OK;
}

// ---------------------------------------------------------------------------
// Test accessors
// ---------------------------------------------------------------------------

const uint8_t *stub_lcd_bitmap(void)     { return s_last_bitmap; }
bool           stub_lcd_draw_called(void){ return s_draw_called; }
void           stub_lcd_reset(void)      { memset(s_last_bitmap, 0, sizeof(s_last_bitmap)); s_draw_called = false; }
