#ifndef UART_H
#define UAR_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "string.h"

// Configuraciones UART
#define TXD (1) // GPIO utilizado para transmitir (TX)
#define RXD (3) // GPIO utilizado para recibir (RX)
#define RTS (UART_PIN_NO_CHANGE) // Sin cambios en el control de flujo RTS
#define CTS (UART_PIN_NO_CHANGE) // Sin cambios en el control de flujo CTS

#define UART_PORT_NUM   (UART_NUM_0) // Número de la UART a utilizar
#define UART_BAUD_RATE  (115200) // Tasa de baudios de la UART
#define TASK_STACK_SIZE (4096) // Tamaño de la pila de tareas
#define BUF_SIZE (1024) // Tamaño del búfer

// Inicialización de la UART
void uart_init(void);

// Tarea para reflejar los datos recibidos en la UART
void echo_task(void *arg);

// Tarea para enviar datos a una cola
void send_to_queue(void *arg);

#endif