#include "acs712.h"
#include <stdint.h>







#define ACS712_V_REF 7.0 // referencia de voltaje 

CurrentVoltageValues acs712_read_current_voltage() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_0); // 0 db para que mida hasta 7V
    
    //configuracion del ADC para el sensor
    uint8_t adc_value = adc1_get_raw(ADC_CHANNEL_6);
    float voltage = (adc_value / 4095.0) * ACS712_V_REF;
    float current = (voltage - ACS712_V_REF / 2.0) / 185.0;

    CurrentVoltageValues values;
    values.current = current;
    values.voltage = voltage;

    return values;
}


