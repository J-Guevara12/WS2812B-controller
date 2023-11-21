#ifndef COLOR_MANAGER_H
#define COLOR_MANAGER_H
#define COLOR_FREQUENCY_HZ 60

#define DEFAULT_MAIN_R 0
#define DEFAULT_MAIN_G 40
#define DEFAULT_MAIN_B 40

#define DEFAULT_BACKGROUND_R 20
#define DEFAULT_BACKGROUND_G 0
#define DEFAULT_BACKGROUND_B 20

#define DEFAULT_COLOR_PATTERN 2

void color_manager_init();
void color_manager_task();

typedef struct {
    int R;
    int G;
    int B;
} Color;

#endif
