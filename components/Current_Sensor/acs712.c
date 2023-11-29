#include "acs712.h"
#include "hal/adc_types.h"
#include <stdint.h>

static bool initialized = false;
static CurrentVoltageValues values;

CurrentVoltageValues acs712_read_current_voltage() {
    if(!initialized){
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11); // 0 db para que mida hasta 7V
        
        //configuracion del ADC para el sensor
    }
    uint8_t adc_value = adc1_get_raw(ADC_CHANNEL_6);
    float voltage = adc_value * 2.45 / (1<<12);

    values.current = voltage;
    values.voltage = (float) adc_value;

    return values;
}


