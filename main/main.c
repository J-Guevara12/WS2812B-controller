#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "nvs_flash.h"
#include "wifi_app.h"
#include "esp_sntp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/queue.h"


#include "adc.h"
#include "led_strip.h"
#include "uart.h"
#include "pattern_generator.h"
#include "color_manager.h"

#include "acs712.h"


//-------------------------------------------------------------------------------
//              DECLARACION DE MEDIR TEMPERATURA

static const char* TAG = "MAIN";

QueueHandle_t enabled_leds_queue;
QueueHandle_t led_strip_queue;

QueueHandle_t current_pattern_queue;
QueueHandle_t current_color_pattern_queue;


QueueHandle_t main_color_queue;
QueueHandle_t secondary_color_queue;
QueueHandle_t background_color_queue;

QueueHandle_t pulse_length_queue;
QueueHandle_t number_of_pulses_queue;
QueueHandle_t period_ms_queue;

void app_main(void){
    CurrentVoltageValues result = acs712_read_current_voltage();
    float current = result.current;
    float voltage = result.voltage;

    // Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
   
    ESP_LOGI(TAG, "current: %2f  Am", current);
    // Inicializa las colas / variables compartidas

    led_strip_init();
    pattern_generator_init();
    color_manager_init();
	wifi_app_start();
    uart_init();

    xTaskCreatePinnedToCore(send_data_task, "Send Data", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(pattern_generator_task, "Pattern Generator", 2048, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(color_manager_task, "Color Manager", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(update_values_uart_task, "UART Manager", 2048, NULL, 3, NULL, 0);

    ESP_LOGI(TAG, "Task creation finished!");

    Color color1 = {0,40,40};
    change_main_color(color1);

    change_pattern(2);
    change_number_of_pulses(1);
    change_pulse_length(10);
    change_color_pattern(2);
}
