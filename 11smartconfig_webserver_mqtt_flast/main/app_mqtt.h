#ifndef __MQTT_APP_H
#define __MQTT_APP_H

typedef void (*mqtt_handle_t) (char *data, int len);
void mqtt_app_init(void);
void mqtt_app_start(void);
void mqtt_set_callback(void *cb);
void mqtt_pub(char *topic, char *data, int len);
void mqtt_sub(char *topic); 
#endif