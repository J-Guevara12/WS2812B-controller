#ifndef COLOR_MANAGER_H
#define COLOR_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#define COLOR_FREQUENCY_HZ 100

#define DEFAULT_MAIN_R 0
#define DEFAULT_MAIN_G 0
#define DEFAULT_MAIN_B 0

#define DEFAULT_SECONDARY_R 0
#define DEFAULT_SECONDARY_G 0
#define DEFAULT_SECONDARY_B 0

#define DEFAULT_BACKGROUND_R 0
#define DEFAULT_BACKGROUND_G 0
#define DEFAULT_BACKGROUND_B 0

#define DEFAULT_COLOR_PATTERN 1
#define MAXIMUM_COLOR_PATTERN 2

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} Color;

void color_manager_init();
void color_manager_task();

bool change_main_color(Color color);
bool change_secondary_color(Color color);
bool change_background_color(Color color);

bool change_color_pattern(int n);

#endif
