#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

//  A non-NULL sentinel used as a fake semaphore handle.
static int s_mutex_sentinel;

SemaphoreHandle_t xSemaphoreCreateMutex(void)
{
    return (SemaphoreHandle_t)&s_mutex_sentinel;
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t sem, TickType_t ticks)
{
    (void)sem;
    (void)ticks;
    return pdTRUE;
}

BaseType_t xSemaphoreGive(SemaphoreHandle_t sem)
{
    (void)sem;
    return pdTRUE;
}

void vSemaphoreDelete(SemaphoreHandle_t sem)
{
    (void)sem;
}

BaseType_t xTaskCreate(TaskFunction_t fn,
                       const char *name,
                       uint32_t stack_depth,
                       void *params,
                       UBaseType_t priority,
                       TaskHandle_t *handle)
{
    //  Do not actually start the task — we only test logic, not the RX loop.
    (void)fn;
    (void)name;
    (void)stack_depth;
    (void)params;
    (void)priority;
    (void)handle;
    return pdPASS;
}
