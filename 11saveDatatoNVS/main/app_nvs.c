#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "app_nvs.h"

#define USER_NAMESPACE "__nvs_user"
#define KEY "config_mode"
#define HOMEID "homeid"

static nvs_handle my_handle;

void app_nvs_set_value(char *key, int value)
{
    int err;
    err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("nvs open fail\n");
    }

    err = nvs_set_i32(my_handle, key, value);
    printf((err != ESP_OK) ? "failed\n" : "Done'n");
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "failed\n" : "Done'n");
    nvs_close(my_handle);
}
void app_nvs_get_value(char *key, int *value)
{
    int err;
    err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("nvs open fail\n");
    }
    err = nvs_get_i32(my_handle, key, value);
    switch (err)
    {
    case ESP_OK:
        printf("Done ok\n");
        printf("%s - %d\n", key, *value);
        break;

    case ESP_ERR_NOT_FOUND:
        printf("The value is not initialized\n");
        break;

    default:
        printf("error (%s) reading!\n", esp_err_to_name(err));
        break;
    }
    nvs_close(my_handle);
}
