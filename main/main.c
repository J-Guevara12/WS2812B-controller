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
#include "display.h"

#include "acs712.h"
#include "freertos/semphr.h"


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

QueueHandle_t current_queue;  // Cola para la intensidad corriente



#define BUTTON_GPIO_PIN GPIO_NUM_21 // Reemplaza GPIO_NUM_0 con el número de pin correcto del botón

SemaphoreHandle_t button_semaphore;
void button_task(void *arg) {
    while (1) {
        if (gpio_get_level(BUTTON_GPIO_PIN) == 0) {
            // El botón está presionado
            xSemaphoreGive(button_semaphore);
            vTaskDelay(pdMS_TO_TICKS(1000)); 
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void stop_program() {
    // Detiene el sistema
    vTaskDelete(NULL);
}

void app_main(void){


    //boton acciones
    button_semaphore = xSemaphoreCreateBinary();
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO_PIN),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // tarea para manejar el botón
    xTaskCreate(button_task, "Button Task", 2048, NULL, 5, NULL);

    while (1) {
        // Esperar hasta que el semáforo del botón se libere
        if (xSemaphoreTake(button_semaphore, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Button pressed! Stopping the program.");
            stop_program();
        }
    }

    ///////////////////////////////////////////////////////////////
    // Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
   
    // Inicializa las colas / variables compartidas
    led_strip_init();
    pattern_generator_init();
    color_manager_init();
	wifi_app_start();
    uart_init();
    display_init();
    adc_init();

    xTaskCreatePinnedToCore(send_data_task, "Send Data", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(pattern_generator_task, "Pattern Generator", 2048, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(color_manager_task, "Color Manager", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(update_values_uart_task, "UART Manager", 2048, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(write_queue, "ADC Current", 2048, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(display_task, "Display_tas", 2048, NULL, 3, NULL, 0);

    ESP_LOGI(TAG, "Task creation finished!");

    Color color1 = {0,40,40};
    change_main_color(color1);

    change_pattern(2);
    change_number_of_pulses(1);
    change_pulse_length(10);
    change_color_pattern(2);
}
