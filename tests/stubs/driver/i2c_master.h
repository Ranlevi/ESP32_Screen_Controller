#pragma once

#include "esp_err.h"

typedef void *i2c_master_bus_handle_t;
typedef int   i2c_port_t;

#define I2C_NUM_0           0
#define I2C_CLK_SRC_DEFAULT 0

typedef struct {
    i2c_port_t i2c_port;
    int        sda_io_num;
    int        scl_io_num;
    int        clk_source;
    int        glitch_ignore_cnt;
    struct {
        unsigned int enable_internal_pullup : 1;
    } flags;
} i2c_master_bus_config_t;

esp_err_t i2c_new_master_bus(const i2c_master_bus_config_t *config,
                              i2c_master_bus_handle_t *ret_handle);
esp_err_t i2c_del_master_bus(i2c_master_bus_handle_t handle);
