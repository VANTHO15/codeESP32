#include <stdio.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include "input_iot.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

input_callback_t input_callback = NULL;
timeoutButton_t timeoutButton_callback = NULL;
static uint64_t _start, _stop, _time_tick_press;
static TimerHandle_t xTimers[1];

static void vTimerCallback(TimerHandle_t xTimer)
{
    uint32_t ID;
    configASSERT(xTimer);
    ID = (uint32_t)pvTimerGetTimerID(xTimer);
    if (ID == 0)
    {
        timeoutButton_callback(18); // timer có id là 0 thì mk tự mặc định là nút nhấn 18 và bên kia nhận số 0 về xử lý
    }
}

static void IRAM_ATTR gpio_input_handler(void *arg)
{
    int gpio_num = (uint32_t)arg;
    uint32_t rtc = xTaskGetTickCountFromISR(); // ms

    if (gpio_get_level(gpio_num) == 0) // sườn xuống
    {
        _start = rtc;
        xTimerStart(xTimers[0], 0);
    }
    else
    { // thả tay ra
        xTimerStop(xTimers[0], 0);
        _stop = rtc;
        _time_tick_press = _stop - _start;
        input_callback(_time_tick_press, gpio_num);
    }
}

void input_io_create(gpio_num_t gpio_num, interrupt_type_edle_t type)
{
    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(gpio_num, type);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, gpio_input_handler, (void *)gpio_num);

    xTimers[0] = xTimerCreate("TimerForTimeout", pdMS_TO_TICKS(5000), pdFALSE, (void *)0, vTimerCallback);
} // false nên chỉ chạy 1 lần thôi

uint8_t input_io_get_level(gpio_num_t gpio_num)
{
    return gpio_get_level(gpio_num);
}

void input_set_callback(void *cb)
{
    input_callback = cb;
}
void input_set_timeout_callback(void *cb)
{
    timeoutButton_callback = cb;
}
