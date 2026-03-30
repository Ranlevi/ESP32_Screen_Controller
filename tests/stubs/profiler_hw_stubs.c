#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdint.h>

//  esp_timer
int64_t esp_timer_get_time(void) { return 0; }

//  esp_system
uint32_t esp_get_free_heap_size(void)        { return 0; }
uint32_t esp_get_minimum_free_heap_size(void){ return 0; }
int      esp_reset_reason(void)              { return 0; }
