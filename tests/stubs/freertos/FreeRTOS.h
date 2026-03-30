#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t TickType_t;
typedef long     BaseType_t;
typedef unsigned long UBaseType_t;

#define pdTRUE   ((BaseType_t)1)
#define pdFALSE  ((BaseType_t)0)
#define pdPASS   pdTRUE

#define portMAX_DELAY     ((TickType_t)0xFFFFFFFFUL)
#define pdMS_TO_TICKS(ms) ((TickType_t)(ms))
