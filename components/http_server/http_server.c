/*
 * http_server.c
 *
 *  Created on: Oct 20, 2021
 *      Author: kjagu
 */

#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "cJSON.h"

#include "esp_wifi_types.h"
#include "http_server.h"
#include "tasks_common.h"
#include "wifi_app.h"
#include <string.h>

// Tag used for ESP serial console messages
static const char TAG[] = "http_server";

// HTTP server task handle
static httpd_handle_t http_server_handle = NULL;

// HTTP server monitor task handle
static TaskHandle_t task_http_server_monitor = NULL;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t http_server_monitor_queue_handle;

extern QueueHandle_t temperatureQueue;

extern QueueHandle_t brightness;


// Embedded files: JQuery, index.html, app.css, app.js and favicon.ico files
extern const uint8_t index_html_start[]				asm("_binary_index_html_start");
extern const uint8_t index_html_end[]				asm("_binary_index_html_end");
extern const uint8_t app_css_start[]				asm("_binary_app_css_start");
extern const uint8_t app_css_end[]					asm("_binary_app_css_end");
extern const uint8_t app_js_start[]					asm("_binary_app_js_start");
extern const uint8_t app_js_end[]					asm("_binary_app_js_end");
extern const uint8_t favicon_ico_start[]			asm("_binary_favicon_ico_start");
extern const uint8_t favicon_ico_end[]				asm("_binary_favicon_ico_end");

/**
 * HTTP server monitor task used to track events of the HTTP server
 * @param pvParameters parameter which can be passed to the task.
 */
static void http_server_monitor(void *parameter)
{
	http_server_queue_message_t msg;

	for (;;)
	{
		if (xQueueReceive(http_server_monitor_queue_handle, &msg, portMAX_DELAY))
		{
			switch (msg.msgID)
			{
				case HTTP_MSG_WIFI_CONNECT_INIT:
					ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_INIT");

					break;

				case HTTP_MSG_WIFI_CONNECT_SUCCESS:
					ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_SUCCESS");

					break;

				case HTTP_MSG_WIFI_CONNECT_FAIL:
					ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_FAIL");

					break;

				case HTTP_MSG_OTA_UPDATE_SUCCESSFUL:
					ESP_LOGI(TAG, "HTTP_MSG_OTA_UPDATE_SUCCESSFUL");

					break;

				case HTTP_MSG_OTA_UPDATE_FAILED:
					ESP_LOGI(TAG, "HTTP_MSG_OTA_UPDATE_FAILED");

					break;

				case HTTP_MSG_OTA_UPATE_INITIALIZED:
					ESP_LOGI(TAG, "HTTP_MSG_OTA_UPATE_INITIALIZED");

					break;

				default:
					break;
			}
		}
	}
}

/**
 * Sends the index.html page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "index.html requested");

	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);

	return ESP_OK;
}

/**
 * app.css get handler is requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_app_css_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "app.css requested");

	httpd_resp_set_type(req, "text/css");
	httpd_resp_send(req, (const char *)app_css_start, app_css_end - app_css_start);

	return ESP_OK;
}

/**
 * app.js get handler is requested when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_app_js_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "app.js requested");

	httpd_resp_set_type(req, "application/javascript");
	httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);

	return ESP_OK;
}

/**
 * Sends the .ico (icon) file when accessing the web page.
 * @param req HTTP request for which the uri needs to be handled.
 * @return ESP_OK
 */
static esp_err_t http_server_favicon_ico_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "favicon.ico requested");

	httpd_resp_set_type(req, "image/x-icon");
	httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start);

	return ESP_OK;
}

static esp_err_t http_server_temperature_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "temperature requested");

	httpd_resp_set_type(req, "application/json");
    char data[100];
    double temp;
    xQueueReceive(temperatureQueue,&temp,(TickType_t)10);
    sprintf(data,"{\"temperature\": \"%f\"}",temp);
	httpd_resp_send(req, data, strlen(data));

	return ESP_OK;
}



static esp_err_t http_server_brightness_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "brightness requested");
	if (req->method == HTTP_POST){
        int total_len = req->content_len;
        int cur_len = 0;
        int received = 0;
		char data [100];
        while (cur_len < total_len){
            received = httpd_req_recv(req, data + cur_len, total_len);
            if (received<0){
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
                return ESP_FAIL;
            }
            cur_len += received;
        }
        data[total_len] = '\0';

        cJSON *root = cJSON_Parse(data);
        int red = cJSON_GetObjectItem(root, "red")->valueint;
        int green = cJSON_GetObjectItem(root, "green")->valueint;
        int blue = cJSON_GetObjectItem(root, "blue")->valueint;

        ESP_LOGI(TAG, "%d, %d, %d", red, green, blue);

        httpd_resp_sendstr(req, "OK");
		return ESP_OK;
    }else{
		httpd_resp_send_404(req);
		return ESP_OK;
	}
}

static esp_err_t http_server_connect_wifi_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Connecting to wifi");
	if (req->method == HTTP_POST){
        int total_len = req->content_len;
        int cur_len = 0;
        int received = 0;
		char data [100];
        while (cur_len < total_len){
            received = httpd_req_recv(req, data + cur_len, total_len);
            if (received<0){
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
                return ESP_FAIL;
            }
            cur_len += received;
        }
        data[total_len] = '\0';

        cJSON *root = cJSON_Parse(data);
        char * ssid = cJSON_GetObjectItem(root, "ssid")->valuestring;
        char * password = cJSON_GetObjectItem(root, "password")->valuestring;

        ESP_LOGI(TAG, "%s, %s", ssid, password);


        wifi_app_send_message(WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER);
        ESP_ERROR_CHECK(esp_wifi_disconnect());

        wifi_config_t* wifi_config = wifi_app_get_wifi_config();
        ESP_LOGI(TAG, "HI!: %s, %s", wifi_config->sta.ssid, wifi_config->sta.password);
        memset(wifi_config,0x00,sizeof(wifi_config_t));
        memcpy(wifi_config->sta.ssid, ssid, strlen(ssid));
        memcpy(wifi_config->sta.password, password, strlen(password));
        ESP_LOGI(TAG, "HEY: %s, %s", wifi_config->sta.ssid, wifi_config->sta.password);
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));			///> Set our configuration


        ESP_ERROR_CHECK(esp_wifi_connect());

		return ESP_OK;
    }else{
		httpd_resp_send_404(req);
		return ESP_OK;
	}

	return ESP_OK;
}


/**
 * Sets up the default httpd server configuration.
 * @return http server instance handle if successful, NULL otherwise.
 */
static httpd_handle_t http_server_configure(void)
{
	// Generate the default configuration
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	// Create HTTP server monitor task
	xTaskCreatePinnedToCore(&http_server_monitor, "http_server_monitor", HTTP_SERVER_MONITOR_STACK_SIZE, NULL, HTTP_SERVER_MONITOR_PRIORITY, &task_http_server_monitor, HTTP_SERVER_MONITOR_CORE_ID);

	// Create the message queue
	http_server_monitor_queue_handle = xQueueCreate(3, sizeof(http_server_queue_message_t));

	// The core that the HTTP server will run on
	config.core_id = HTTP_SERVER_TASK_CORE_ID;

	// Adjust the default priority to 1 less than the wifi application task
	config.task_priority = HTTP_SERVER_TASK_PRIORITY;

	// Bump up the stack size (default is 4096)
	config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;

	// Increase uri handlers
	config.max_uri_handlers = 20;

	// Increase the timeout limits
	config.recv_wait_timeout = 10;
	config.send_wait_timeout = 10;

	ESP_LOGI(TAG,
			"http_server_configure: Starting server on port: '%d' with task priority: '%d'",
			config.server_port,
			config.task_priority);

	// Start the httpd server
	if (httpd_start(&http_server_handle, &config) == ESP_OK)
	{
		ESP_LOGI(TAG, "http_server_configure: Registering URI handlers");

		// register index.html handler
		httpd_uri_t index_html = {
				.uri = "/",
				.method = HTTP_GET,
				.handler = http_server_index_html_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &index_html);

		// register app.css handler
		httpd_uri_t app_css = {
				.uri = "/app.css",
				.method = HTTP_GET,
				.handler = http_server_app_css_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &app_css);

		// register app.js handler
		httpd_uri_t app_js = {
				.uri = "/app.js",
				.method = HTTP_GET,
				.handler = http_server_app_js_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &app_js);

		// register favicon.ico handler
		httpd_uri_t favicon_ico = {
				.uri = "/favicon.ico",
				.method = HTTP_GET,
				.handler = http_server_favicon_ico_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &favicon_ico);

		httpd_uri_t temperature_json = {
				.uri = "/api/temperature",
				.method = HTTP_GET,
				.handler = http_server_temperature_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &temperature_json);


		httpd_uri_t brightness_json = {
				.uri = "/api/brightness",
				.method = HTTP_POST,
				.handler = http_server_brightness_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &brightness_json);

		httpd_uri_t connect_wifi  = {
				.uri = "/api/connect",
				.method = HTTP_POST,
				.handler = http_server_connect_wifi_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &connect_wifi);


		

		return http_server_handle;
	}

	return NULL;
}

void http_server_start(void)
{
	if (http_server_handle == NULL)
	{
		http_server_handle = http_server_configure();
	}
}

void http_server_stop(void)
{
	if (http_server_handle)
	{
		httpd_stop(http_server_handle);
		ESP_LOGI(TAG, "http_server_stop: stopping HTTP server");
		http_server_handle = NULL;
	}
	if (task_http_server_monitor)
	{
		vTaskDelete(task_http_server_monitor);
		ESP_LOGI(TAG, "http_server_stop: stopping HTTP server monitor");
		task_http_server_monitor = NULL;
	}
}

BaseType_t http_server_monitor_send_message(http_server_message_e msgID)
{
	http_server_queue_message_t msg;
	msg.msgID = msgID;
	return xQueueSend(http_server_monitor_queue_handle, &msg, portMAX_DELAY);
}



















