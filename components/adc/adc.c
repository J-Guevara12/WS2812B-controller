#include <adc.h>
#include <math.h>
#include "esp_err.h"
#include "esp_log.h"

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
