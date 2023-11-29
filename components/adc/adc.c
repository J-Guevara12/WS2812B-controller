#include <adc.h>
#include <math.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define UPDATE_PERIOD 20  // Periodo de actulizacion 
#define THRESHOLD_VALUE 300

#include "display.h"
#include "adc.h"


static const char *TAG = "ADC"; 

extern QueueHandle_t current_queue;  // Cola para la intensidad corriente

adc_oneshot_unit_handle_t adc_handler;


// Inicializacion del ADC
void adc_init(void){
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT,
        .ulp_mode = ADC_ULP,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc_handler));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handler, ADC_CHAN, &config));

    current_queue = xQueueCreate(1, sizeof(float));

    ESP_LOGI(TAG,"ADC intialized");
}



void write_queue(){
    int val;
    float current;
    while (true){
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handler, ADC_CHAN, &val));
        current = (float) val / 4096;
        current = CALIBRATION_M * current + CALIBRATION_B;
        xQueueOverwrite(current_queue,&current);  // Envia el valor de la intesidad del lED a la cola
        check_threshold(val);
        vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD)); 
    }
}


void check_threshold(int value) {
    float current_threshold;
    
    // recibir un valor de la cola current_queue
    if (xQueueReceive(current_queue, &current_threshold, 0) == pdPASS) {
        ESP_LOGI(TAG, "Received current threshold from queue: %f", current_threshold);
        
        if (current_threshold > THRESHOLD_VALUE) {
            ESP_LOGI(TAG, "Threshold exceeded! Taking action...");
            
            show_warning_on_display();
        } else {
            
            clear_display();
        }
    } else {
        
        ESP_LOGE(TAG, "Failed to receive current threshold from queue");
    }
}
