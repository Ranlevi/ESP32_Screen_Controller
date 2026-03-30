#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

typedef void *esp_lcd_panel_io_handle_t;

typedef struct {
    int dev_addr;
    int scl_speed_hz;
    int control_phase_bytes;
    int dc_bit_offset;
    int lcd_cmd_bits;
    int lcd_param_bits;
} esp_lcd_panel_io_i2c_config_t;

esp_err_t esp_lcd_new_panel_io_i2c(i2c_master_bus_handle_t bus,
                                    const esp_lcd_panel_io_i2c_config_t *io_config,
                                    esp_lcd_panel_io_handle_t *ret_io);
esp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io);
