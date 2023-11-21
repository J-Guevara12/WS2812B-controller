#include "pattern_generator.h"
#include "freertos/projdefs.h"
#include "led_strip.h"
#include "portmacro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"


#include <math.h>

QueueHandle_t number_of_pulses_queue;
QueueHandle_t period_ms_queue;

static const char *TAG = "LED_STRIP";

extern QueueHandle_t pulse_length_queue;
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

    xQueueOverwrite(pulse_length_queue, &default_pulse_length );
    xQueueOverwrite(number_of_pulses_queue, &default_number_of_pulses );
    xQueueOverwrite(period_ms_queue, &default_period_ms );
    xQueueOverwrite(current_pattern_queue, &default_pattern );

    ESP_LOGI(TAG, "Pattern generator initializated!");
}

void constant(bool state){
    bool enabled_leds[NUMBER_OF_LEDS] = {[0 ... NUMBER_OF_LEDS - 1] = state};
    xQueueOverwrite(enabled_leds_queue, &enabled_leds );
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
    xQueueOverwrite(enabled_leds_queue, &enabled_leds );

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
    xQueueOverwrite(enabled_leds_queue, &enabled_leds );

}

void pattern_generator_task(){
    ESP_LOGI(TAG,"Started pattern generator task");
    TickType_t LastWakeTime = xTaskGetTickCount();
    int current_step = 0;
    int current_pattern = 0;
    int delta = 1;
    xQueuePeek(current_pattern_queue, &current_pattern, (TickType_t) 10);
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
