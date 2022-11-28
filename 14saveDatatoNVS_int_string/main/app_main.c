
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define KEY "int_nvs"
#define KEY1 "string_nsv"
#include "app_nvs.h"


void app_main(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    int count = 0 ;
    app_nvs_get_value(KEY, &count);
    count++;
    app_nvs_set_value(KEY, count);

    char value_set[100];
    sprintf(value_set,"ThoNV %d", count);
    char value[100];
    app_nvs_get_string(KEY1, value);
    app_nvs_set_string(KEY1,value_set);




  
}

