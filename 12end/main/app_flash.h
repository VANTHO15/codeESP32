#ifndef __APP_FLASH_H
#define __APP_FLASH_H
int app_flash_get_config_mode(void);
void app_flash_set_config_mode(int mode);
char* app_flash_get_home_id(void);
void app_flash_set_home_id(char *homeID);
#endif 