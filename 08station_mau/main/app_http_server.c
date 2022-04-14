#include "app_http_server.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
#include <string.h>

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */
extern const uint8_t index_html_start[] asm("_binary_webserver_html_start");
extern const uint8_t index_html_end[] asm("_binary_webserver_html_end");

static const char *TAG = "example";
static httpd_handle_t server = NULL;

static switch_handle_t p_switch_handle = NULL;
static slider_handle_t p_slider_handle = NULL;
static rgb_handler_t p_rgb_handle = NULL;

/* An HTTP GET handler */
static esp_err_t http_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    httpd_resp_set_type(req, "text/html");
    const char* resp_str = (const char*) index_html_start;
    httpd_resp_send(req, resp_str, index_html_end - index_html_start);
    return ESP_OK; 
}

static const httpd_uri_t http_get = {
    .uri       = "/get",
    .method    = HTTP_GET,
    .handler   = http_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};

/* An HTTP GET handler */
static esp_err_t dht11_handler(httpd_req_t *req)
{
    char res[100]= "";
    float temp = 50.25;
    float humi = 80.91;
    sprintf(res,"{\"temperature\": \"%.02f\", \"humidity\": \"%.02f\"}",temp,humi);
    httpd_resp_send(req, res, strlen(res));
    return ESP_OK;
}

static const httpd_uri_t http_dht11 = {
    .uri       = "/dht11",
    .method    = HTTP_GET,
    .handler   = dht11_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};

/* An HTTP GET handler */
static esp_err_t rgb_handler(httpd_req_t *req)
{
    char res[100]= "";
    size_t buf_len;
    char param[7];
    char val[3] = {0,0,0};
    int r,g,b;
    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        if (httpd_req_get_url_query_str(req, res, buf_len) == ESP_OK) {
            /* Get value of expected key from query string */
            if (httpd_query_key_value(res, "color", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
                char*s;
                val[0] = param[0];
                val[1] = param[1];
                r = strtol(val, &s, 16);

                val[0] = param[2];
                val[1] = param[3];
                g = strtol(val, &s, 16);

                val[0] = param[4];
                val[1] = param[5];
                b = strtol(val, &s, 16);       

                ESP_LOGI(TAG, "r = %d, g = %d, b = %d", r,g,b);
                p_rgb_handle(r,g,b);                   
            }             
        }
    } 

    httpd_resp_send(req, res, strlen(res));
    return ESP_OK;
}

static const httpd_uri_t http_rgb = {
    .uri       = "/rgb",
    .method    = HTTP_GET,
    .handler   = rgb_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};

/* An HTTP POST handler */
static esp_err_t http_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, data_len = req->content_len;

        /* Read the data for the request */
        httpd_req_recv(req, buf,data_len);
        /* Log data received */
        ESP_LOGI(TAG, "Data recv: %.*s", data_len, buf);

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t http_post = {
    .uri       = "/post",
    .method    = HTTP_POST,
    .handler   = http_post_handler,
    .user_ctx  = NULL
};


/* An HTTP POST handler */
static esp_err_t sw1_handler(httpd_req_t *req)
{
    char buf[100];
    int led_value;
    int ret, data_len = req->content_len;

        /* Read the data for the request */
        httpd_req_recv(req, buf,data_len);
        /* Log data received */
        ESP_LOGI(TAG, "Data recv: %.*s", data_len, buf);
        led_value = atoi(buf);
        p_switch_handle(led_value);
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t http_sw1 = {
    .uri       = "/sw1",
    .method    = HTTP_POST,
    .handler   = sw1_handler,
    .user_ctx  = NULL
};

/* An HTTP POST handler */
static esp_err_t slider_handler(httpd_req_t *req)
{
    char buf[100];
    int duty_value;
    int ret, data_len = req->content_len;

        /* Read the data for the request */
        httpd_req_recv(req, buf,data_len);
        /* Log data received */
        duty_value = atoi(buf);
        p_slider_handle(duty_value);
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t http_slider = {
    .uri       = "/slider",
    .method    = HTTP_POST,
    .handler   = slider_handler,
    .user_ctx  = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
// esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
// {
//     if (strcmp("/hello", req->uri) == 0) {
//         httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
//         /* Return ESP_OK to keep underlying socket open */
//         return ESP_OK;
//     } else if (strcmp("/echo", req->uri) == 0) {
//         httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
//         /* Return ESP_FAIL to close underlying socket */
//         return ESP_FAIL;
//     }
//     /* For any other URI send 404 and close socket */
//     httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
//     return ESP_FAIL;
// }

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &http_get);
        httpd_register_uri_handler(server, &http_post);
        httpd_register_uri_handler(server, &http_dht11);
        httpd_register_uri_handler(server, &http_sw1);
        httpd_register_uri_handler(server, &http_slider);
        httpd_register_uri_handler(server, &http_rgb);
    }
}

void stop_webserver(void)
{
    // Stop the httpd server
    httpd_stop(server);
}

void switch_set_callback(void *cb)
{
    if(cb)
        p_switch_handle = cb;
}

void slider_set_callback(void *cb)
{
    if(cb)
        p_slider_handle = cb;
}

void rgb_set_callback(void *cb)
{
    if(cb)
        p_rgb_handle = cb;
}