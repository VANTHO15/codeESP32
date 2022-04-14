#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include "ledc_io.h"

void app_main(void)
{
    pwm_init(2);
    while (1) {
        for(int i=0;i<100;i++)
        {
            pwm_set_duty(i);
            vTaskDelay(100 / portTICK_PERIOD_MS); 
        }
    }
}
