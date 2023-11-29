#include <string.h>
#include <sys/time.h>
#include "freertos/projdefs.h"
#include "math.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "portmacro.h"
#include "ssd1306.h"
#include "display.h"
#include "esp_log.h"

static const char* TAG = "DISPLAY";
static i2c_config_t conf;
static ssd1306_handle_t ssd1306_dev = NULL;

extern QueueHandle_t current_queue;  // Cola para la intensidad corriente

void display_init(){


    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_refresh_gram(ssd1306_dev);

    ESP_LOGI(TAG,"Display initialized");
}

void test(){
    char data_str[32] = {};
    sprintf(data_str, "Hello world!");
    ssd1306_draw_string(ssd1306_dev, 0, 0, (const uint8_t *)data_str, 16, 0);
    ssd1306_refresh_gram(ssd1306_dev);
}

void display_task(){
    char data_str[32] = {};
    float current;
    while (true){
        sprintf(data_str, "Current: %2f mA",current);
        //current = round(current*100)/100;
        xQueuePeek(current_queue, &current, (TickType_t) 10);
        ssd1306_draw_string(ssd1306_dev, 0, 0, (const uint8_t *)data_str, 16, 0);
        ssd1306_refresh_gram(ssd1306_dev);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}


void show_warning_on_display() {
    // Mensaje de advertencia
    char warning_message[20];
    snprintf(warning_message, sizeof(warning_message), "Warning!");

    // Limpia la pantalla y muestra el mensaje
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_draw_string(ssd1306_dev, 0, 0, (const uint8_t *)warning_message, 16, 1); // El último parámetro (1) indica que se utilizará la fuente grande
    ssd1306_refresh_gram(ssd1306_dev);
}

void clear_display() {
    // Borra la pantalla
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_refresh_gram(ssd1306_dev);
}


