#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "led_strip.h"

static const char *TAG = "WS2812B";
static led_strip_t *strip;
static int number_led = 0;

void ws2812_init(int tx_pin, int number)
{
    number_led = number;
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(tx_pin, RMT_CHANNEL_0);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(number, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip) {
        ESP_LOGE(TAG, "install WS2812 driver failed");
    }
    vTaskDelay(10/portTICK_PERIOD_MS);
}

void ws2812_set_rgb(int index, int r, int g, int b) // max value = 255
{
    // Write RGB values to strip driver
    ESP_ERROR_CHECK(strip->set_pixel(strip, index, r, g, b));
    // Flush RGB values to LEDs
    ESP_ERROR_CHECK(strip->refresh(strip, 100));   
    vTaskDelay(10 / portTICK_PERIOD_MS);  
}

void ws2812_set_all_rgb(int r, int g, int b)
{
    for(int i =0; i<number_led;i++)
    {
        ESP_ERROR_CHECK(strip->set_pixel(strip, i, r, g, b));
    }
    ESP_ERROR_CHECK(strip->refresh(strip, 100)); 
}

void ws2812_set_off(void)
{
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));    
}