#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "input_iot.h"
#include "output_iot.h"

#define LED 2
#define BTN 18

void HamGoiCallBack(int pin)
{
  if (pin == BTN)
  {
    for(int i =1;i<50000;i++);
    while(input_io_get_level(BTN) == 0);
    for(int i =1;i<50000;i++);
    output_io_toggle(LED);
  }
}

void app_main(void)
{
  output_io_create(LED);

  input_io_create(BTN, HI_TO_LO);
  input_set_callback(HamGoiCallBack);
  
}

