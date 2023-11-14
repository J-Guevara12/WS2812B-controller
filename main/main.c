#include "nvs_flash.h"
#include "wifi_app.h"
#include "esp_sntp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/queue.h"


#include "adc.h"
#include "led_strip.h"
#include "uart.h"

//-------------------------------------------------------------------------------
//              DECLARACION DE MEDIR TEMPERATURA

static const char* TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG,"Finished Task creation");
    led_strip_init();
    send_data();
}
