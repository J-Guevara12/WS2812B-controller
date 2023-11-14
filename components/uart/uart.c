#include "uart.h"

static const char *TAG = "UART";


//Llamado de cola  Uart  para enviar datos a otras tareas
extern QueueHandle_t uartDataQueue; // cola uart


//Inicializacion de la UART
void uart_init(){
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configuracion de los parametros de la UART
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM,BUF_SIZE*2,0,0,NULL,0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, TXD, RXD, RTS, CTS));

    ESP_LOGI(TAG,"Initializated UART");
}


// Tarea: Leer datos de la UART y  env  arlos de vuelta
void echo_task(void *arg){
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Leer datos de la UART
        int len = uart_read_bytes(UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        // Escribir datos de vuelta en la UART
        uart_write_bytes(UART_PORT_NUM, (const char *) data, len);
        if (len) {
            data[len] = '\0';
            ESP_LOGI(TAG, "Recv str: %s", (char *) data);
        }
    }
}
