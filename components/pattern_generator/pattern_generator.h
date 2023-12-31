#ifndef PATTERN_GENERATOR_H
#define PATTERN_GENERATOR_H

#include <stdint.h>
#include "led_strip.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define DEFAULT_PULSE_LENGTH 4
#define DEFAULT_NUMBER_OF_PULSES 3
#define DEFAULT_PERIOD_MS 100.0
#define DEFAULT_PATTERN 1
#define MAXIMUM_PATTERN 4

void pattern_generator_init();
void pattern_generator_task();

bool change_pulse_length(int n);
bool change_number_of_pulses(int n);
bool change_change_period_ms(float t);
bool change_pattern(int n);

#endif
