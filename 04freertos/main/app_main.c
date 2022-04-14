
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "output_iot.h"
#include "freertos/timers.h"

#define LED 2

TimerHandle_t xTimers[2];
void vTimerCallback(TimerHandle_t xTimer)
{
    uint32_t ulCount;
    configASSERT(xTimer);
    ulCount = (uint32_t)pvTimerGetTimerID(xTimer);
    if (ulCount == 0)
    {
        output_io_toggle(LED);
    }
    else if (ulCount == 1)
    {
        printf("Van Tho\n");
    }
}

void vTask1(void *pvParameters)
{
    for (;;)
    {
        printf("Task1\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
void vTask2(void *pvParameters)
{
    for (;;)
    {
        printf("Task2\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
void vTask3(void *pvParameters)
{
    for (;;)
    {
        printf("Task3\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
void app_main(void)
{
    xTimers[0] = xTimerCreate("Timer", pdMS_TO_TICKS(500), pdTRUE, (void *)0, vTimerCallback);
    xTimers[1] = xTimerCreate("Timer", pdMS_TO_TICKS(500), pdTRUE, (void *)1, vTimerCallback);

    output_io_create(LED);

    xTimerStart(xTimers[0], 0);
    xTimerStart(xTimers[1], 0);

    xTaskCreate(vTask1, "Task1", 1024, NULL, 4, NULL); // 4 trở lên, càng cao càng ưu tiên
    xTaskCreate(vTask2, "Task2", 1024, NULL, 5, NULL);
    xTaskCreate(vTask3, "Task3", 1024, NULL, 6, NULL);
}

