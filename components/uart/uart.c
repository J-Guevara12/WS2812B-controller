#include "uart.h"
#include "color_manager.h"
#include "pattern_generator.h"
#include <string.h>

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


// Tarea: Leer datos de la UART y  enviarlos de vuelta
void update_values_uart_task(void *arg){
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    int index = 0;

    while (1) {
        // Leer datos de la UART
        int len = uart_read_bytes(UART_PORT_NUM, data+index, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        // Escribir datos de vuelta en la UART
        uart_write_bytes(UART_PORT_NUM, (const char *) data, len);
        if (len) {
            data[index + len] = '\0';
            if(strcmp((char *) &data[index+len-1]," ")==0){

                char * command = strtok((char *) &data[0], "_");
                if(command){
                    if(strcmp(command,"pl")==0){
                        ESP_LOGI(TAG,"Changing pulse length");
                        char * value_token = strtok(NULL, "_");
                        int n = atoi(value_token);
                        if(n>0){
                            if(change_pulse_length(n)){
                                ESP_LOGI(TAG, "Pulse length changed to: %d", n);
                            }
                        }
                    }
                    else if(strcmp(command,"np")==0){
                        ESP_LOGI(TAG,"Changing number of pulses");
                        char * value_token = strtok(NULL, "_");
                        int n = atoi(value_token);
                        if(n>0){
                            if(change_number_of_pulses(n)){
                                ESP_LOGI(TAG, "Number of pulses changed to: %d", n);
                            }
                        }
                    }
                    else if(strcmp(command,"pat")==0){
                        ESP_LOGI(TAG,"Changing pattern");
                        char * value_token = strtok(NULL, "_");
                        int n = atoi(value_token);
                        if(change_pattern(n)){
                            ESP_LOGI(TAG, "Pattern changed to: %d", n);
                        }
                    }
                    else if(strcmp(command,"mc")==0){
                        ESP_LOGI(TAG,"Changing main color");
                        char * value_token = strtok(NULL, "_");
                        Color color = {0,0,0};
                        if(value_token){
                            color.R = atoi(value_token);
                            value_token = strtok(NULL, "_");
                            if(value_token){
                                color.G = atoi(value_token);
                                value_token = strtok(NULL, "_");
                                if(value_token){
                                    color.B = atoi(value_token);
                                }
                            }
                        }
                        if(change_main_color(color)){
                            ESP_LOGI(TAG, "Main Color changed to (%d,%d,%d)",color.R,color.G,color.B);
                        }
                    }
                    else if(strcmp(command,"sc")==0){
                        ESP_LOGI(TAG,"Changing secondary color");
                        char * value_token = strtok(NULL, "_");
                        Color color = {0,0,0};
                        if(value_token){
                            color.R = atoi(value_token);
                            value_token = strtok(NULL, "_");
                            if(value_token){
                                color.G = atoi(value_token);
                                value_token = strtok(NULL, "_");
                                if(value_token){
                                    color.B = atoi(value_token);
                                }
                            }
                        }
                        if(change_secondary_color(color)){
                            ESP_LOGI(TAG, "Secondary Color changed to (%d,%d,%d)",color.R,color.G,color.B);
                        }
                    }
                    else if(strcmp(command,"bc")==0){
                        ESP_LOGI(TAG,"Changing background color");
                        char * value_token = strtok(NULL, "_");
                        Color color = {0,0,0};
                        if(value_token){
                            color.R = atoi(value_token);
                            value_token = strtok(NULL, "_");
                            if(value_token){
                                color.G = atoi(value_token);
                                value_token = strtok(NULL, "_");
                                if(value_token){
                                    color.B = atoi(value_token);
                                }
                            }
                        }
                        if(change_background_color(color)){
                            ESP_LOGI(TAG, "Background Color changed to (%d,%d,%d)",color.R,color.G,color.B);
                        }
                    }
                    else if(strcmp(command,"cp")==0){
                        ESP_LOGI(TAG,"Changing color pattern");
                        char * value_token = strtok(NULL, "_");
                        int n = atoi(value_token);
                        if(change_pattern(n)){
                            ESP_LOGI(TAG, "Color pattern changed to: %d", n);
                        }
                    }
                    else {
                        ESP_LOGI(TAG,"Unknown command: %s",command);
                    }
                }

                index = 0;
                len = 0;
                memset(data, '\0', BUF_SIZE);
            }
            index += len;
        }
    }
}
// Tarea: Leer datos de la UART y  enviarlos de vuelta
void echo_task(void *arg){
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    int index = 0;

    while (1) {
        // Leer datos de la UART
        int len = uart_read_bytes(UART_PORT_NUM, data+index, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        // Escribir datos de vuelta en la UART
        uart_write_bytes(UART_PORT_NUM, (const char *) data, len);
        if (len) {
            data[index + len] = '\0';
            if(strcmp((char *) &data[index+len-1],".")==0){
                ESP_LOGI(TAG, "Recv str: %s", (char *) data);
                index = 0;
                len = 0;
                memset(data, '\0', BUF_SIZE);
            }
            index += len;
        }
    }
}
