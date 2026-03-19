#include "app_key.h"

#include "cmsis_os.h"
#include "main.h"

typedef enum {
    KEY_STATE_IDLE = 0,
    KEY_STATE_DEBOUNCE,
    KEY_STATE_PRESSED,
    KEY_STATE_LONG_SENT,
} KeyState_t;

typedef struct {
    KeyState_t state;
    uint16_t ticks;
    KeyCmd_t short_cmd;
    KeyCmd_t long_cmd;
} KeyCtx_t;

extern osMessageQueueId_t Queue_CmdHandle;

#define KEY_SCAN_PERIOD_MS 10u
#define KEY_DEBOUNCE_MS 30u
#define KEY_LONG_PRESS_MS 800u

#define KEY_DEBOUNCE_TICKS (KEY_DEBOUNCE_MS / KEY_SCAN_PERIOD_MS)
#define KEY_LONG_TICKS (KEY_LONG_PRESS_MS / KEY_SCAN_PERIOD_MS)

#define KEY_PRESSED GPIO_PIN_RESET

#define KEY1_READ() HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define KEY2_READ() HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)
#define KEY3_READ() HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin)
#define KEY4_READ() HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin)

static KeyCtx_t s_keys[4] = {
    {KEY_STATE_IDLE, 0u, KEY_CMD_KEY1_SHORT, KEY_CMD_KEY1_LONG},
    {KEY_STATE_IDLE, 0u, KEY_CMD_KEY2_SHORT, KEY_CMD_NONE},
    {KEY_STATE_IDLE, 0u, KEY_CMD_KEY3_SHORT, KEY_CMD_NONE},
    {KEY_STATE_IDLE, 0u, KEY_CMD_KEY4_SHORT, KEY_CMD_NONE},
};

static GPIO_PinState App_Key_Read(uint8_t idx)
{
    switch (idx) {
    case 0u:
        return KEY1_READ();
    case 1u:
        return KEY2_READ();
    case 2u:
        return KEY3_READ();
    case 3u:
        return KEY4_READ();
    default:
        return GPIO_PIN_SET;
    }
}

static void App_Key_Send(KeyCmd_t cmd)
{
    uint8_t raw;

    if (cmd == KEY_CMD_NONE) {
        return;
    }

    raw = (uint8_t)cmd;
    (void)osMessageQueuePut(Queue_CmdHandle, &raw, 0u, 0u);
}

static void App_Key_UpdateOne(KeyCtx_t *key, GPIO_PinState level)
{
    switch (key->state) {
    case KEY_STATE_IDLE:
        if (level == KEY_PRESSED) {
            key->state = KEY_STATE_DEBOUNCE;
            key->ticks = 0u;
        }
        break;

    case KEY_STATE_DEBOUNCE:
        if (level == KEY_PRESSED) {
            key->ticks++;
            if (key->ticks >= KEY_DEBOUNCE_TICKS) {
                key->state = KEY_STATE_PRESSED;
                key->ticks = 0u;
            }
        } else {
            key->state = KEY_STATE_IDLE;
            key->ticks = 0u;
        }
        break;

    case KEY_STATE_PRESSED:
        if (level != KEY_PRESSED) {
            App_Key_Send(key->short_cmd);
            key->state = KEY_STATE_IDLE;
            key->ticks = 0u;
        } else {
            key->ticks++;
            if ((key->long_cmd != KEY_CMD_NONE) && (key->ticks >= KEY_LONG_TICKS)) {
                App_Key_Send(key->long_cmd);
                key->state = KEY_STATE_LONG_SENT;
                key->ticks = 0u;
            }
        }
        break;

    case KEY_STATE_LONG_SENT:
        if (level != KEY_PRESSED) {
            key->state = KEY_STATE_IDLE;
            key->ticks = 0u;
        }
        break;

    default:
        key->state = KEY_STATE_IDLE;
        key->ticks = 0u;
        break;
    }
}

void App_Key_Init(void)
{
    uint32_t i;
    for (i = 0u; i < 4u; i++) {
        s_keys[i].state = KEY_STATE_IDLE;
        s_keys[i].ticks = 0u;
    }
}

void App_Key_Run(void)
{
    uint8_t i;

    for (i = 0u; i < 4u; i++) {
        App_Key_UpdateOne(&s_keys[i], App_Key_Read(i));
    }

    osDelay(KEY_SCAN_PERIOD_MS);
}
