#include "color_manager.h"
#include "led_strip.h"
#include "pattern_generator.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "portmacro.h"
#include <stdint.h>

static const char *TAG = "LED_STRIP";

extern QueueHandle_t enabled_leds_queue;
extern QueueHandle_t main_color_queue;
extern QueueHandle_t background_color_queue;
extern QueueHandle_t pulse_length_queue;
extern QueueHandle_t current_color_pattern_queue;

void color_manager_init(){
    main_color_queue = xQueueCreate(1, sizeof(Color));
    background_color_queue = xQueueCreate(1, sizeof(Color));
    current_color_pattern_queue = xQueueCreate(1, sizeof(int));

    Color main_default_color = {DEFAULT_MAIN_R, DEFAULT_MAIN_G, DEFAULT_MAIN_B};
    Color background_default_color = {DEFAULT_BACKGROUND_R, DEFAULT_BACKGROUND_G, DEFAULT_BACKGROUND_B};
    int default_color_pattern = DEFAULT_COLOR_PATTERN;

    xQueueOverwrite(main_color_queue, &main_default_color);
    xQueueOverwrite(background_color_queue, &background_default_color);
    xQueueOverwrite(current_color_pattern_queue, &default_color_pattern);

};

void constant_color(int n, Color color){
    set_color(n, color.R, color.G, color.B);
}

void faded(int n, Color color, Color background, int pos_in_pulse, int pulse_length){
    int R = (uint8_t) background.R + ((pos_in_pulse)*(color.R - background.R )/ pulse_length);
    int G = (uint8_t) background.G + ((pos_in_pulse)*(color.G - background.G )/ pulse_length);
    int B = (uint8_t) background.B + ((pos_in_pulse)*(color.B - background.B )/ pulse_length);
    set_color(n, R, G, B);
}

void color_manager_task(){
    TickType_t LastWakeTime = xTaskGetTickCount();
    float increment =  1000.0 * 1 / COLOR_FREQUENCY_HZ;
    bool enabled_leds[NUMBER_OF_LEDS];

    int current_color_pattern = 0;
    Color main_color = {0,0,0};
    Color background_color = {0,0,0};

    int pos_in_pulse = 0;
    int pulse_length = 1;

    ESP_LOGI(TAG,"Started color manager task");
    while (true){
        // Update values from queues
        xQueuePeek(enabled_leds_queue, &enabled_leds, (TickType_t) 10);

        xQueuePeek(main_color_queue, &main_color, (TickType_t) 10);
        xQueuePeek(background_color_queue, &background_color, (TickType_t) 10);
        xQueuePeek(current_color_pattern_queue, &current_color_pattern, (TickType_t) 10);

        xQueuePeek(pulse_length_queue, &pulse_length, (TickType_t) 10);

        for(int n = 0; n < NUMBER_OF_LEDS; n++){
            if(enabled_leds[n]){
                switch (current_color_pattern) {
                    case 1:
                        constant_color(n, main_color);
                        break;
                    case 2:
                        faded(n,main_color, background_color, ++pos_in_pulse%(pulse_length+1),pulse_length);
                        break;
                    default:
                        constant_color(n, background_color);
                }
            }
            else{
                constant_color(n, background_color);
                pos_in_pulse = 0;
            }
        }
        vTaskDelayUntil(&LastWakeTime, pdMS_TO_TICKS(increment));
    }
}
