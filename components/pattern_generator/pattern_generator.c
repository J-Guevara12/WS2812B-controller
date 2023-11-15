#include "pattern_generator.h"
#include "freertos/projdefs.h"
#include "led_strip.h"
#include "portmacro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"


#include <math.h>

QueueHandle_t pulse_length_queue;
QueueHandle_t number_of_pulses_queue;
QueueHandle_t period_ms_queue;

static const char *TAG = "LED_STRIP";

extern QueueHandle_t enabled_leds_queue;
extern QueueHandle_t current_pattern_queue;

void pattern_generator_init(){
    enabled_leds_queue = xQueueCreate(1, NUMBER_OF_LEDS * sizeof(bool));
    pulse_length_queue = xQueueCreate(1, sizeof(int));
    number_of_pulses_queue = xQueueCreate(1, sizeof(int));
    period_ms_queue = xQueueCreate(1, sizeof(float));
    current_pattern_queue = xQueueCreate(1, sizeof(int));

    int default_pulse_length = DEFAULT_PULSE_LENGTH;
    int default_number_of_pulses = DEFAULT_NUMBER_OF_PULSES;
    int default_period_ms = DEFAULT_PERIOD_MS;
    int default_pattern = DEFAULT_PATTERN;

    xQueueSend(pulse_length_queue, &default_pulse_length, (TickType_t) 10);
    xQueueSend(number_of_pulses_queue, &default_number_of_pulses, (TickType_t) 10);
    xQueueSend(period_ms_queue, &default_period_ms, (TickType_t) 10);
    xQueueSend(current_pattern_queue, &default_pattern, (TickType_t) 10);

    ESP_LOGI(TAG, "Pattern generator initializated!");
}

void constant(bool state){
    bool enabled_leds[NUMBER_OF_LEDS] = {[0 ... NUMBER_OF_LEDS - 1] = state};
    xQueueSend(enabled_leds_queue, &enabled_leds, (TickType_t) 10);
}

void pulses(int * i){
    bool enabled_leds[NUMBER_OF_LEDS] = {[0 ... NUMBER_OF_LEDS - 1] = false};
    int pulse_length = 0;
    int number_of_pulses = 0;
    xQueuePeek(pulse_length_queue, &pulse_length, (TickType_t) 10);
    xQueuePeek(number_of_pulses_queue, &number_of_pulses, (TickType_t) 10);

    if(pulse_length*number_of_pulses > NUMBER_OF_LEDS) {
        ESP_LOGI(TAG,"The size of the pulses is greater than the led strip");
        return;
    }

    int space = (NUMBER_OF_LEDS-pulse_length*number_of_pulses)/(number_of_pulses);
    // Iterates over each pixel in the strip
    for (int j = 0; j< number_of_pulses; j++){
        // Iterates over each pulse
        for(int k = 0; k < pulse_length; k++){
            // Iterates over each pixel in the pulse
            enabled_leds[(*i+k+(space+pulse_length)*j)%NUMBER_OF_LEDS] = true;
        }
    }
    (*i)++;
    *i%=NUMBER_OF_LEDS;
    xQueueSend(enabled_leds_queue, &enabled_leds, (TickType_t) 10);

}

void swing(int * i, int * delta){
    bool enabled_leds[NUMBER_OF_LEDS] = {[0 ... NUMBER_OF_LEDS - 1] = false};
    int pulse_length = 0;
    int number_of_pulses = 0;
    xQueuePeek(pulse_length_queue, &pulse_length, (TickType_t) 10);
    xQueuePeek(number_of_pulses_queue, &number_of_pulses, (TickType_t) 10);

    if(pulse_length*number_of_pulses > NUMBER_OF_LEDS) {
        ESP_LOGI(TAG,"The size of the pulses is greater than the led strip");
        return;
    }

    int space = (NUMBER_OF_LEDS-pulse_length*number_of_pulses)/(number_of_pulses);
    // Iterates over each pixel in the strip
    for (int j = 0; j< number_of_pulses; j++){
        // Iterates over each pulse
        for(int k = 0; k < pulse_length; k++){
            // Iterates over each pixel in the pulse
            enabled_leds[(*i+k+(space+pulse_length)*j)%NUMBER_OF_LEDS] = true;
            if((*i+k+(space+pulse_length)*j)%NUMBER_OF_LEDS == NUMBER_OF_LEDS -1){
                *delta=-1;
            }
        }
    }
    if(*i==0){
        *delta = 1;
    }
    *i += *delta;
    *i%=NUMBER_OF_LEDS;
    xQueueSend(enabled_leds_queue, &enabled_leds, (TickType_t) 10);

}

void pattern_generator_task(){
    ESP_LOGI(TAG,"Started pattern generator task");
    TickType_t LastWakeTime = xTaskGetTickCount();
    int current_step = 0;
    int current_pattern = 0;
    int delta = 1;
    xQueueReceive(current_pattern_queue, &current_pattern, (TickType_t) 10);
    float period_ms = DEFAULT_PERIOD_MS;
    while (true){
        switch(current_pattern){
            case 1:
                constant(true);
                break;
            case 2:
                pulses(&current_step);
                break;
            case 3:
                swing(&current_step, &delta);
                break;
            default:
                constant(false);
        }
        vTaskDelayUntil(&LastWakeTime, pdMS_TO_TICKS(period_ms));
    }
}



// ------------------PATRON DE BARRIDO ---------------------------------
void sweep(int *i, int *delta) {
    bool enabled_leds[NUMBER_OF_LEDS] = {[0 ... NUMBER_OF_LEDS - 1] = false};
    int pulse_length = 0;
    int number_of_pulses = 0;
    xQueuePeek(pulse_length_queue, &pulse_length, (TickType_t) 10);
    xQueuePeek(number_of_pulses_queue, &number_of_pulses, (TickType_t) 10);

    if (pulse_length * number_of_pulses > NUMBER_OF_LEDS) {
        ESP_LOGI(TAG, "The size of the pulses is greater than the led strip");
        return;
    }

    int space = (NUMBER_OF_LEDS - pulse_length * number_of_pulses) / (number_of_pulses);

    // Obtiene el tiempo actual en microsegundos
    uint32_t currentTime = esp_timer_get_time();

    // Declara la variable 'lastUpdate'
    static uint32_t lastUpdate = 0;

    // Calcula la diferencia de tiempo desde la última actualización
    uint32_t elapsedTime = currentTime - lastUpdate;

    // Actualiza la posición según el tiempo transcurrido
    *i += (*delta) * (elapsedTime / 10000);  // Divide por 10,000 para convertir de microsegundos a milisegundos

    //  Se Calcula la posición del barrido en función del tiempo que pasa
    static int position = 0;

    for (int j = 0; j < number_of_pulses; j++) {
        // Itera sobre cada pulso
        for (int k = 0; k < pulse_length; k++) {
            // Calcula la posición del barrido para el LED actual
            int sweep_position = (*i + k + (space + pulse_length) * j) % NUMBER_OF_LEDS;

            // Calcula la intensidad en función de la posición relativa al barrido
            uint8_t intensity = abs(position - sweep_position) < 10 ? 255 : 0;

            // Establece el color en el LED actual
            enabled_leds[sweep_position] = true;
        }
    }

    // Envía el arreglo de LEDs habilitados a la cola
    xQueueSend(enabled_leds_queue, &enabled_leds, (TickType_t) 10);

    // Actualiza la posición para el próximo ciclo
    if (*i == 0) {
        *delta = 1;
    }
    *i %= NUMBER_OF_LEDS;

    // Actualiza el tiempo de la última actualización
    lastUpdate = currentTime;
}



void rainbow(int *i) {
    bool enabled_leds[NUMBER_OF_LEDS] = {[0 ... NUMBER_OF_LEDS - 1] = false};
    int pulse_length = 0;
    int number_of_pulses = 0;
    xQueuePeek(pulse_length_queue, &pulse_length, (TickType_t) 10);
    xQueuePeek(number_of_pulses_queue, &number_of_pulses, (TickType_t) 10);

    if (pulse_length * number_of_pulses > NUMBER_OF_LEDS) {
        ESP_LOGI(TAG, "The size of the pulses is greater than the led strip");
        return;
    }

    int space = (NUMBER_OF_LEDS - pulse_length * number_of_pulses) / (number_of_pulses);

    for (int j = 0; j < number_of_pulses; j++) {
        // Itera sobre cada pulso
        for (int k = 0; k < pulse_length; k++) {
            // Calcula la posición del arcoíris para el LED actual
            int rainbow_position = (*i + k + (space + pulse_length) * j) % NUMBER_OF_LEDS;

            // Calcula un valor de color basado en la posición
            uint8_t hue = (rainbow_position * 255) / NUMBER_OF_LEDS;

            // Calcula los componentes RGB del color del arco iris
            uint8_t red = 0, green = 0, blue = 0;

            if (hue < 85) {
                red = hue * 3;
                green = 255 - hue * 3;
            } else if (hue < 170) {
                hue -= 85;
                red = 255 - hue * 3;
                blue = hue * 3;
            } else {
                hue -= 170;
                green = hue * 3;
                blue = 255 - hue * 3;
            }

            // Establece el color en el LED actual
            uint32_t color = (red << 16) | (green << 8) | blue;
            enabled_leds[rainbow_position] = true;
        }
    }

    // Actualiza la posición para el próximo ciclo
    (*i)++;
    (*i) %= NUMBER_OF_LEDS;

    // Envía el arreglo de LEDs habilitados a la cola
    xQueueSend(enabled_leds_queue, &enabled_leds, (TickType_t) 10);
}


void random_colors(int *i) {
    bool enabled_leds[NUMBER_OF_LEDS] = {[0 ... NUMBER_OF_LEDS - 1] = false};
    int pulse_length = 0;
    int number_of_pulses = 0;
    xQueuePeek(pulse_length_queue, &pulse_length, (TickType_t) 10);
    xQueuePeek(number_of_pulses_queue, &number_of_pulses, (TickType_t) 10);

    if (pulse_length * number_of_pulses > NUMBER_OF_LEDS) {
        ESP_LOGI(TAG, "The size of the pulses is greater than the led strip");
        return;
    }

    int space = (NUMBER_OF_LEDS - pulse_length * number_of_pulses) / (number_of_pulses);

    for (int j = 0; j < number_of_pulses; j++) {
        // Itera sobre cada pulso
        for (int k = 0; k < pulse_length; k++) {
            // Calcula la posición para el LED actual
            int position = (*i + k + (space + pulse_length) * j) % NUMBER_OF_LEDS;

            // Genera valores de color aleatorios
            uint8_t red = (uint8_t)rand() % 256;
            uint8_t green = (uint8_t)rand() % 256;
            uint8_t blue = (uint8_t)rand() % 256;
            
            // Establece el color en el LED actual
            enabled_leds[position] = true;
        }
    }

    // Actualiza la posición para el próximo ciclo
    (*i)++;
    (*i) %= NUMBER_OF_LEDS;

    // Envía el arreglo de LEDs habilitados a la cola
    xQueueSend(enabled_leds_queue, &enabled_leds, (TickType_t) 10);
}






void sin_wave(int *i, int *delta) {
    bool enabled_leds[NUMBER_OF_LEDS] = {[0 ... NUMBER_OF_LEDS - 1] = false};
    int pulse_length = 0;
    int number_of_pulses = 0;
    xQueuePeek(pulse_length_queue, &pulse_length, (TickType_t) 10);
    xQueuePeek(number_of_pulses_queue, &number_of_pulses, (TickType_t) 10);

    if (pulse_length * number_of_pulses > NUMBER_OF_LEDS) {
        ESP_LOGI(TAG, "The size of the pulses is greater than the led strip");
        return;
    }

    int space = (NUMBER_OF_LEDS - pulse_length * number_of_pulses) / (number_of_pulses);

    // Obtiene el tiempo actual en microsegundos
    uint32_t currentTime = esp_timer_get_time();

    // Declara la variable 'lastUpdate'
    static uint32_t lastUpdate = 0;

    // Calcula la diferencia de tiempo desde la última actualización
    uint32_t elapsedTime = currentTime - lastUpdate;

    // Actualiza la posición según el tiempo transcurrido
    *i += (*delta) * (elapsedTime / 1000);  // Divide por 1000 para convertir de microsegundos a milisegundos

    // Calcula la posición de la onda sinusoidal para cada LED
    for (int j = 0; j < number_of_pulses; j++) {
        for (int k = 0; k < pulse_length; k++) {
            int position = (*i + k + (space + pulse_length) * j) % NUMBER_OF_LEDS;

            // Calcula la intensidad en función de la posición de la onda sinusoidal
            uint8_t intensity = (uint8_t)(128 + 127 * sin(position * 2 * M_PI / NUMBER_OF_LEDS));

            // Establece el color en el LED actual
            enabled_leds[position] = true;
        }
    }

    // Envía el arreglo de LEDs habilitados a la cola
    xQueueSend(enabled_leds_queue, &enabled_leds, (TickType_t) 10);

    // Actualiza la posición para el próximo ciclo
    if (*i == 0) {
        *delta = 1;
    }
    *i %= NUMBER_OF_LEDS;

    // Actualiza el tiempo de la última actualización
    lastUpdate = currentTime;
}
