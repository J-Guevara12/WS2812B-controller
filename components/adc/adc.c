#include <adc.h>
#include <math.h>
#include "esp_err.h"
#include "esp_log.h"

#include "display.h"
#include "ssd1306.h"

ssd1306_handle_t ssd1306_dev;

#define UPDATE_PERIOD 20   // Periodo de actulizacion 20 milisegundos

static const char *TAG = "ADC"; //llamado de colas externas  

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



void show_warning_on_display() {
    
    char warning_message[20];
    snprintf(warning_message, sizeof(warning_message), "Warning!");

  
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_draw_string(ssd1306_dev, 0, 0, (const uint8_t *)warning_message, 16, 1); 
    ssd1306_refresh_gram(ssd1306_dev);


    vTaskDelay(pdMS_TO_TICKS(3000));

 
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_refresh_gram(ssd1306_dev);
}

//Tarea: Escribe valores en la cola a  utilizar
void write_queue(){
    int val;
    float current;
    while (true){
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handler, ADC_CHAN, &val));
        current = (float) val / 4096;
        current = CALIBRATION_M * current + CALIBRATION_B;
        xQueueOverwrite(current_queue,&current);  // Envia el valor de la intesidad del lED a la cola
        vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD)); // Tiempo de espera para la proxima lectura
    }
}


#define THRESHOLD_VALUE 500

void check_threshold(int value) {
    if (value > THRESHOLD_VALUE) {
        ESP_LOGI(TAG, "Threshold exceeded! Taking action...");
        
         show_warning_on_display();  
    }
}

