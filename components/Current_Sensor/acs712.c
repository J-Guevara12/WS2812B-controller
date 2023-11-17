

#include "acs712.h"



float  acs712_read_current(){

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC1_ATTEN_DB_0); // 0 db para que mida hasta 7V
    
    //configuracion del ADC para el sensor
    uint8_t adc_value = adc1_get(ADC_CHANNEL_6);
    
    float voltaje = (adc_value/ 4095.0) *  ACS712_V_REF; 

    float current = (voltaje - ACS712_V_REF/2.0)/185.0 ;

    return current;



}


