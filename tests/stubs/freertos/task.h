#pragma once

#include "FreeRTOS.h"

typedef void *TaskHandle_t;
typedef void (*TaskFunction_t)(void *);

BaseType_t  xTaskCreate(TaskFunction_t fn,
                        const char *name,
                        uint32_t stack_depth,
                        void *params,
                        UBaseType_t priority,
                        TaskHandle_t *handle);
UBaseType_t uxTaskGetNumberOfTasks(void);
void        vTaskGetRunTimeStats(char *buf);
void        vTaskDelay(TickType_t ticks);
