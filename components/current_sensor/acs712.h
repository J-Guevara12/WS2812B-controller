#ifndef ACS712_H
#define ACS712_H


#include "driver/adc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



#define  ACS712_SENSOR_PIN  34  //pin a conectar
#define ACS712_V_REF      1100  //  valor de referencia (mV)

typedef struct {
    float current;
    float voltage;
} CurrentVoltageValues;


CurrentVoltageValues acs712_read_current_voltage();

#endif   






