#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "input_iot.h"
#include "output_iot.h"

#include "freertos/event_groups.h"

#define LED 2
#define BTN 18

EventGroupHandle_t xEventGroup;
#define BIT_PRESS_1000 (1 << 0) // có 13 cái liền, tha hồ làm, nếu k đủ thì tạo thêm 1 event group nữa
#define BIT_PRESS_1000_3000 (1 << 1)
#define BIT_PRESS_3000 (1 << 2)

void vTask1(void *pvParameters)
{
    for (;;)
    {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(
            xEventGroup,                                  /* The event group being tested. */
            BIT_PRESS_1000 | BIT_PRESS_1000_3000|BIT_PRESS_3000, /* The bits within the event group to wait for. */
            pdTRUE,                                       /* BIT_0 & BIT_4 should be cleared before returning. */
            pdFALSE,                                      /* Don't wait for both bits, either bit will do. */
            portMAX_DELAY);                               /* Wait a maximum of 100ms for either bit to be set. */


        if (uxBits & BIT_PRESS_1000)
        {
            printf("1000\n");
            output_io_toggle(LED);
        }
        if (uxBits & BIT_PRESS_1000_3000)
        {
            printf("1000-3000\n");
        }
        if (uxBits & BIT_PRESS_3000)
        {
            printf("3000\n");
        }
    }
}
void button_timeout_callback(int pin)
{
    if(pin == GPIO_NUM_18)
    {
        output_io_toggle(LED);
        printf("TIMEOUT\n");
    }
}

// 1 tick = 10ms
// N tick = ? ms

void HamGoiCallBack(uint64_t timeTick,int pin)
{
  if (pin == BTN)
  {
    BaseType_t pxHigherPriorityTaskWoken;
    int press_ms = timeTick*portTICK_PERIOD_MS;
    if(press_ms < 1000)
    {
        xEventGroupSetBitsFromISR(xEventGroup, BIT_PRESS_1000, &pxHigherPriorityTaskWoken);
    }
    else if((press_ms > 1000) && (press_ms < 3000))
    {
        xEventGroupSetBitsFromISR(xEventGroup, BIT_PRESS_1000_3000, &pxHigherPriorityTaskWoken);
    }
    else if(press_ms > 3000)
    {
        xEventGroupSetBitsFromISR(xEventGroup, BIT_PRESS_3000, &pxHigherPriorityTaskWoken);
    }
    output_io_toggle(LED);
  }
}

void app_main(void)
{
  output_io_create(LED);

  input_io_create(BTN, ANY_EDLE);
  input_set_callback(HamGoiCallBack);
  input_set_timeout_callback(button_timeout_callback);

  xEventGroup = xEventGroupCreate();

  xTaskCreate(vTask1, "Task1", 1024 * 3, NULL, 4, NULL); // 4 trở lên, càng cao càng ưu tiên
  
}

