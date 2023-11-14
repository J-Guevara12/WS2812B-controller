#ifndef ADC_H
#define ADC_H

#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"


// Definiciones de configuración ADC
#define ADC_UNIT ADC_UNIT_1
#define ADC_ULP ADC_ULP_MODE_DISABLE
#define ADC_BITWIDTH ADC_BITWIDTH_13
#define ADC_ATTEN ADC_ATTEN_DB_11
#define ADC_CHAN ADC_CHANNEL_0


// Inicialización del ADC
void adc_init(void);

// Función para escribir datos en una cola
void write_queue();

#endif
