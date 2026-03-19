#ifndef APP_KEY_H
#define APP_KEY_H

#include <stdint.h>

typedef enum {
    KEY_CMD_NONE = 0x00,
    KEY_CMD_KEY1_SHORT = 0x11,
    KEY_CMD_KEY2_SHORT = 0x12,
    KEY_CMD_KEY3_SHORT = 0x13,
    KEY_CMD_KEY4_SHORT = 0x14,
    KEY_CMD_KEY1_LONG = 0x21,
} KeyCmd_t;

void App_Key_Init(void);
void App_Key_Run(void);

#endif
