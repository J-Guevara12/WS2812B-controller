#include "color_manager.h"
#include "freertos/projdefs.h"
#include "led_strip.h"
#include "pattern_generator.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "portmacro.h"

static const char *TAG = "LED_STRIP";

extern QueueHandle_t enabled_leds_queue;

void color_manager_task(){
    TickType_t LastWakeTime = xTaskGetTickCount();
    float increment =  1000.0 * 1 / COLOR_FREQUENCY_HZ;
    bool enabled_leds[NUMBER_OF_LEDS];
    ESP_LOGI(TAG,"Started color manager task %f", increment);
    while (true){
        xQueueReceive(enabled_leds_queue, &enabled_leds, (TickType_t) 10);
        for(int n = 0; n < NUMBER_OF_LEDS; n++){
            if(enabled_leds[n]){
                set_color(n, 100, 0, 100);
            }
            else{
                set_color(n, 0, 0, 0);
            }
        }
        vTaskDelayUntil(&LastWakeTime, pdMS_TO_TICKS(increment));
    }
}
