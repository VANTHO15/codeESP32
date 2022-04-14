
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "output_iot.h"
#include "freertos/timers.h"
#include "input_iot.h"
#include "output_iot.h"

#include "freertos/event_groups.h"
// dữ liệu khi mà từ ngắt sẽ đẩy qua 1 hàm event group rồi mới xử lý

#define LED 2
#define BTN 18

EventGroupHandle_t xEventGroup;
#define BIT_EVENT_BUTTON_PRESS (1 << 0) // có 13 cái liền, tha hồ làm, nếu k đủ thì tạo thêm 1 event group nữa
#define BIT_EVENT_UART_RECV (1 << 1)

void vTask1(void *pvParameters)
{
    for (;;)
    {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(
            xEventGroup,                                  /* The event group being tested. */
            BIT_EVENT_BUTTON_PRESS | BIT_EVENT_UART_RECV, /* The bits within the event group to wait for. */
            pdTRUE,                                       /* BIT_0 & BIT_4 should be cleared before returning. */
            pdFALSE,                                      /* Don't wait for both bits, either bit will do. */
            portMAX_DELAY);                               /* Wait a maximum of 100ms for either bit to be set. */


        if (uxBits & BIT_EVENT_BUTTON_PRESS)
        {
            printf("BUTTON PRESS\n");
            output_io_toggle(LED);
            // Xử lý dữ liệu ở đây
        }
        if (uxBits & BIT_EVENT_UART_RECV)
        {
            printf("UART DATA\n");
            // Xử lý dữ liệu ở đây
        }
    }
}

void Button_callback(int pin)
{
    if (pin == BTN)
    {
         for(int i =1;i<50000;i++);
        while(input_io_get_level(BTN) == 0);
        for(int i =1;i<50000;i++);
        
        BaseType_t pxHigherPriorityTaskWoken;
        xEventGroupSetBitsFromISR(xEventGroup, BIT_EVENT_BUTTON_PRESS, &pxHigherPriorityTaskWoken);
    }
}
void app_main(void)
{
    output_io_create(LED);

    input_io_create(BTN, HI_TO_LO);
    input_set_callback(Button_callback);

    xEventGroup = xEventGroupCreate();

    xTaskCreate(vTask1, "Task1", 1024 * 3, NULL, 4, NULL); // 4 trở lên, càng cao càng ưu tiên
}
