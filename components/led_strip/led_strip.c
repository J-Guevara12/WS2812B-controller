#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/rmt_tx.h"

#include "led_strip_encoder.h"
#include "led_strip.h"
#include "portmacro.h"
#include <stdint.h>

rmt_channel_handle_t led_chan = NULL;
rmt_encoder_handle_t led_encoder = NULL;

rmt_transmit_config_t tx_config = {
    .loop_count = 0, // no transfer loop
};

static const char *TAG = "LED_STRIP";

extern QueueHandle_t led_strip_queue;

void led_strip_init() {
        rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = RMT_LED_STRIP_GPIO_NUM, // Set the GPIO Pin
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .resolution_hz =  RMT_LED_STRIP_RESOLUTION_HZ,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
    };

    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &led_chan));
    led_strip_encoder_config_t encoder_config = {
        .resolution = RMT_LED_STRIP_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));
    ESP_ERROR_CHECK(rmt_enable(led_chan));
    ESP_LOGI(TAG, "LED strip initializated!");

    led_strip_queue = xQueueCreate(1, 3 * NUMBER_OF_LEDS * sizeof(uint8_t));
}

void set_color(int n, uint8_t R, uint8_t G, uint8_t B){
    static uint8_t led_strip[3*NUMBER_OF_LEDS];
    xQueuePeek(led_strip_queue, &led_strip, (TickType_t) 10);
    led_strip[n*3 + 1] = R;
    led_strip[n*3] = G;
    led_strip[n*3 + 2] = B;
    xQueueOverwrite( led_strip_queue, led_strip );
}

void send_data(){
    static uint8_t led_strip[3*NUMBER_OF_LEDS];
    xQueuePeek(led_strip_queue, &led_strip, (TickType_t) 10);
    ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, &led_strip, 3*NUMBER_OF_LEDS, &tx_config));
}

void send_data_task(){
    TickType_t LastWakeTime = xTaskGetTickCount();
    float incremet =  1000.0 * 1 / LED_STRIP_FREQUENCY_HZ;
    ESP_LOGI(TAG,"Started data sending task");
    while (true){
        send_data();
        vTaskDelayUntil(&LastWakeTime, pdMS_TO_TICKS(incremet));
    }
}
