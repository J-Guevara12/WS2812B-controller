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

//-------------------------------------------------------------------------------
//              DECLARACION DE MEDIR TEMPERATURA

static const char* TAG = "MAIN";

QueueHandle_t enabled_leds_queue;
QueueHandle_t current_pattern_queue;
QueueHandle_t led_strip_queue;


void app_main(void)
{
    led_strip_init();
    pattern_generator_init();
    xTaskCreatePinnedToCore(send_data_task, "Send Data", 2048, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(pattern_generator_task, "Pattern Generator", 2048, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(color_manager_task, "Color Manager", 2048, NULL, 3, NULL, 0);
    ESP_LOGI(TAG, "Task creation finished!");
}
