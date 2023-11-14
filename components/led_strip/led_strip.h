#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <stdint.h>
#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define RMT_LED_STRIP_MEM_BLOCK_SYMBOLS 64  // increase the block size can make the LED less flickering
#define RMT_LED_STRIP_TRANS_QUEUE_DEPTH 4  // set the number of transactions that can be pending in the background
#define RMT_LED_STRIP_GPIO_NUM 15         // Output pin
#define NUMBER_OF_LEDS 16                // LEDS to control

#include "esp_log.h"
#include "driver/rmt_tx.h"

static uint8_t led_strip[3*NUMBER_OF_LEDS] = {0};

void led_strip_init();

void send_data();

void set_color(int n, int R, int G, int B);
void set_red(int n, int R);
void set_green(int n, int G);
void set_blue(int n, int B);

#endif
