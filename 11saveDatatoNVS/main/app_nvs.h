#ifndef __APP_FLASH_H
#define __APP_FLASH_H
void app_nvs_get_value(char *key, int *value);
void app_nvs_set_value(char *key, int value);
#endif