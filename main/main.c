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
void rainbow(int *current_step);

static const char* TAG = "MAIN";

QueueHandle_t enabled_leds_queue;
QueueHandle_t current_pattern_queue;
QueueHandle_t led_strip_queue;

void app_main(void){
    CurrentVoltageValues result = acs712_read_current_voltage();
    float current = result.current;
    float voltage = result.voltage;
   
    ESP_LOGI(TAG, "current: %2f  Am", current);
    // Inicializa las colas / variables compartidas
    if (enabled_leds_queue == NULL || current_pattern_queue == NULL || led_strip_queue == NULL) {
        ESP_LOGE(TAG, "Error creating queues");
        return;
    }

    led_strip_init();
    pattern_generator_init();

    xTaskCreatePinnedToCore(send_data_task, "Send Data", 2048, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(pattern_generator_task, "Pattern Generator", 2048, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(color_manager_task, "Color Manager", 2048, NULL, 3, NULL, 0);
    
    ESP_LOGI(TAG, "Task creation finished!");
}
