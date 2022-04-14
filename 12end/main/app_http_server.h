#ifndef __APP_HTTP_SERVER_H
#define __APP_HTTP_SERVER_H

typedef void (*http_post_handle_t) (char *data, int len);
void start_webserver(void);
void stop_webserver(void);
void http_post_set_callback(void *cb);
#endif