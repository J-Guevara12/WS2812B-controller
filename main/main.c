#include "nvs_flash.h"
#include "wifi_app.h"
#include "esp_sntp.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/queue.h"


#include "adc.h"
#include "uart.h"

//-------------------------------------------------------------------------------
//              DECLARACION DE MEDIR TEMPERATURA

#define INTERRUPT_GPIO 13
static const char* TAG = "MAIN";

void app_main(void)
{
	//--------------------------------------------------------------------------
	//               CONEXION WIFI Y NTP

	// Start Wifi
	wifi_app_start();

    //star ntp
    //Set_SystemTime_SNTP();

    //----------------------------------------------------------------------------
	//            MEDIR TEMPERATURA

    gpio_set_direction(INTERRUPT_GPIO,GPIO_MODE_INPUT);
    gpio_set_pull_mode(INTERRUPT_GPIO,GPIO_PULLUP_ONLY);


    // Habilitar interrupcion  negatica / flaco de bajada
    gpio_intr_enable(INTERRUPT_GPIO);
    gpio_set_intr_type(INTERRUPT_GPIO,GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INTERRUPT_GPIO,isr,NULL);

     // Tareas

    ESP_LOGI(TAG,"Finished Task creation");
}
