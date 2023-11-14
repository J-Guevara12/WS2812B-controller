#include "esp_log.h"
#include "driver/rmt_tx.h"

#include "led_strip_encoder.h"
#include "led_strip.h"

rmt_channel_handle_t led_chan = NULL;
rmt_encoder_handle_t led_encoder = NULL;

rmt_transmit_config_t tx_config = {
    .loop_count = 0, // no transfer loop
};

static const char *TAG = "LED_STRIP";

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
}

void send_data(){
    ESP_ERROR_CHECK(rmt_transmit(led_chan, led_encoder, &led_strip, 3*NUMBER_OF_LEDS, &tx_config));
}
