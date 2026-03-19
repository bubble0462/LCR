#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

#include <stdint.h>

typedef enum {
    SCREEN_MEASURE = 0,
    SCREEN_MENU,
    SCREEN_SET_FREQ,
    SCREEN_SET_AMP,
    SCREEN_SET_DISPMODE,
    SCREEN_ABOUT,
} ScreenId_t;

typedef enum {
    DISP_MODE_NORMAL = 0,
    DISP_MODE_DETAIL,
} DispMode_t;

void App_Display_Init(void);
void App_Display_Run(void);

#endif
