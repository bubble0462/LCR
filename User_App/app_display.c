#include "app_display.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "OLED.h"
#include "app_key.h"
#include "app_measure.h"
#include "bsp_afe.h"

extern osMessageQueueId_t Queue_CmdHandle;

static const uint32_t kFreqList[] = {100u, 1000u, 10000u, 100000u};
static const char *kFreqStr[] = {"100Hz", "1kHz", "10kHz", "100kHz"};

static const char *kAmpStr[] = {"0.3Vr", "0.6Vr", "1.0Vr"};

static const char *kMenuItems[] = {
    "PinLv(Freq)",
    "ZhenFu(Amp)",
    "XianShi(Mode)",
    "GuanYu(About)",
};

static ScreenId_t s_screen = SCREEN_MEASURE;
static DispMode_t s_disp_mode = DISP_MODE_NORMAL;

static uint8_t s_menu_index = 0u;
static uint8_t s_freq_index = 1u;
static uint8_t s_amp_index = 0u;

static uint16_t s_bat_ticks = 0u;
static float s_bat_v = 3.7f;

static MeasureResult_t s_last_result;

static const char *App_Display_RrefText(AFE_Rref_t rref)
{
    switch (rref) {
    case RREF_20R:
        return "20R";
    case RREF_1K:
        return "1k";
    case RREF_10K:
        return "10k";
    case RREF_100K:
        return "100k";
    default:
        return "?";
    }
}

static void App_Display_FormatRes(float value, char *buf, uint32_t len)
{
    float absv = fabsf(value);

    if (absv >= 1e6f) {
        snprintf(buf, len, "%.3fMR", value / 1e6f);
    } else if (absv >= 1e3f) {
        snprintf(buf, len, "%.3fkR", value / 1e3f);
    } else if (absv >= 1.0f) {
        snprintf(buf, len, "%.3fR", value);
    } else {
        snprintf(buf, len, "%.2fmR", value * 1e3f);
    }
}

static void App_Display_FormatCap(float value, char *buf, uint32_t len)
{
    float absv = fabsf(value);

    if (absv >= 1e-3f) {
        snprintf(buf, len, "%.3fmF", value * 1e3f);
    } else if (absv >= 1e-6f) {
        snprintf(buf, len, "%.3fuF", value * 1e6f);
    } else if (absv >= 1e-9f) {
        snprintf(buf, len, "%.3fnF", value * 1e9f);
    } else {
        snprintf(buf, len, "%.2fpF", value * 1e12f);
    }
}

static void App_Display_FormatInd(float value, char *buf, uint32_t len)
{
    float absv = fabsf(value);

    if (absv >= 1.0f) {
        snprintf(buf, len, "%.3fH", value);
    } else if (absv >= 1e-3f) {
        snprintf(buf, len, "%.3fmH", value * 1e3f);
    } else if (absv >= 1e-6f) {
        snprintf(buf, len, "%.3fuH", value * 1e6f);
    } else {
        snprintf(buf, len, "%.2fnH", value * 1e9f);
    }
}

static void App_Display_FormatPrimary(const MeasureResult_t *result, char *buf, uint32_t len)
{
    char val[20];

    if ((result == NULL) || (result->valid == 0u)) {
        snprintf(buf, len, "-- Measuring --");
        return;
    }

    switch (result->comp_type) {
    case COMP_RESISTOR:
        App_Display_FormatRes(result->primary_value, val, sizeof(val));
        snprintf(buf, len, "R %s", val);
        break;
    case COMP_CAPACITOR:
        App_Display_FormatCap(result->primary_value, val, sizeof(val));
        snprintf(buf, len, "C %s", val);
        break;
    case COMP_INDUCTOR:
        App_Display_FormatInd(result->primary_value, val, sizeof(val));
        snprintf(buf, len, "L %s", val);
        break;
    default:
        snprintf(buf, len, "Unknown");
        break;
    }
}

static void App_Display_BatteryBar(float vbat, char *buf, uint32_t len)
{
    int pct = (int)(((vbat - 3.5f) / (4.2f - 3.5f)) * 100.0f);
    int bars;

    if (pct < 0) {
        pct = 0;
    }
    if (pct > 100) {
        pct = 100;
    }

    bars = pct / 20;
    snprintf(buf,
             len,
             "[%c%c%c%c%c]",
             (bars >= 1) ? '#' : '.',
             (bars >= 2) ? '#' : '.',
             (bars >= 3) ? '#' : '.',
             (bars >= 4) ? '#' : '.',
             (bars >= 5) ? '#' : '.');
}

static void App_Display_DrawMeasure(const MeasureResult_t *result)
{
    char line[32];
    char zbuf[16];
    char bat[12];

    App_Display_FormatPrimary(result, line, sizeof(line));
    OLED_ShowString(0, 0, line, OLED_8X16);

    if ((result != NULL) && (result->valid != 0u)) {
        snprintf(line, sizeof(line), "Q=%.2f D=%.3f", result->q_factor, result->d_factor);
    } else {
        snprintf(line, sizeof(line), "Q=--- D=---");
    }
    OLED_ShowString(0, 16, line, OLED_6X8);

    if ((result != NULL) && (result->valid != 0u)) {
        App_Display_FormatRes(result->z_mag_ohm, zbuf, sizeof(zbuf));
        snprintf(line, sizeof(line), "|Z|=%s", zbuf);
        OLED_ShowString(0, 24, line, OLED_6X8);

        snprintf(line, sizeof(line), "Ph=%.1f", result->phase_deg);
        OLED_ShowString(72, 24, line, OLED_6X8);

        snprintf(line,
                 sizeof(line),
                 "R:%s G:%u",
                 App_Display_RrefText(result->rref_range),
                 (unsigned)result->pga_gain);
        OLED_ShowString(0, 32, line, OLED_6X8);
    } else {
        OLED_ShowString(0, 24, "|Z|=---", OLED_6X8);
        OLED_ShowString(72, 24, "Ph=---", OLED_6X8);
        OLED_ShowString(0, 32, "R:-- G:--", OLED_6X8);
    }

    App_Display_BatteryBar(s_bat_v, bat, sizeof(bat));
    snprintf(line, sizeof(line), "%s %s", kFreqStr[s_freq_index], bat);
    OLED_ShowString(0, 40, line, OLED_6X8);

    snprintf(line, sizeof(line), "Bat=%.2fV", s_bat_v);
    OLED_ShowString(0, 48, line, OLED_6X8);

    if (s_disp_mode == DISP_MODE_NORMAL) {
        OLED_ShowString(0, 56, "K1Menu K4Freq K2Mode", OLED_6X8);
    } else {
        if ((result != NULL) && (result->valid != 0u)) {
            App_Display_FormatRes(result->re_ohm, zbuf, sizeof(zbuf));
            snprintf(line, sizeof(line), "Re=%s", zbuf);
            OLED_ShowString(0, 56, line, OLED_6X8);

            App_Display_FormatRes(result->im_ohm, zbuf, sizeof(zbuf));
            snprintf(line, sizeof(line), "Im=%s", zbuf);
            OLED_ShowString(64, 56, line, OLED_6X8);
        } else {
            OLED_ShowString(0, 56, "Re=--- Im=---", OLED_6X8);
        }
    }
}

static void App_Display_DrawMenu(void)
{
    uint8_t i;

    OLED_ShowString(0, 0, "Menu(CaiDan)", OLED_6X8);
    OLED_ShowString(0, 8, "K2/K3 move, K1 ok", OLED_6X8);

    for (i = 0u; i < 4u; i++) {
        char line[24];
        snprintf(line, sizeof(line), "%c %s", (i == s_menu_index) ? '>' : ' ', kMenuItems[i]);
        OLED_ShowString(0, (uint8_t)(16u + i * 8u), line, OLED_6X8);
    }

    OLED_ShowString(0, 56, "K1 long: back", OLED_6X8);
}

static void App_Display_DrawSetFreq(void)
{
    char line[24];

    OLED_ShowString(0, 0, "PinLv(Freq)", OLED_6X8);
    OLED_ShowString(0, 8, "K2/K3 change", OLED_6X8);
    OLED_ShowString(0, 16, "K1 save, long back", OLED_6X8);

    snprintf(line, sizeof(line), "> %s", kFreqStr[s_freq_index]);
    OLED_ShowString(0, 32, line, OLED_8X16);
}

static void App_Display_DrawSetAmp(void)
{
    char line[24];

    OLED_ShowString(0, 0, "ZhenFu(Amp)", OLED_6X8);
    OLED_ShowString(0, 8, "K2/K3 change", OLED_6X8);
    OLED_ShowString(0, 16, "K1 save, long back", OLED_6X8);

    snprintf(line, sizeof(line), "> %s", kAmpStr[s_amp_index]);
    OLED_ShowString(0, 32, line, OLED_8X16);
}

static void App_Display_DrawSetMode(void)
{
    OLED_ShowString(0, 0, "XianShi(Mode)", OLED_6X8);
    OLED_ShowString(0, 8, "K2/K3 toggle", OLED_6X8);
    OLED_ShowString(0, 16, "K1 save, long back", OLED_6X8);

    if (s_disp_mode == DISP_MODE_NORMAL) {
        OLED_ShowString(0, 32, "> NORMAL", OLED_8X16);
    } else {
        OLED_ShowString(0, 32, "> DETAIL", OLED_8X16);
    }
}

static void App_Display_DrawAbout(void)
{
    OLED_ShowString(0, 0, "GuanYu(About)", OLED_6X8);
    OLED_ShowString(0, 8, "LCR Meter v1", OLED_6X8);
    OLED_ShowString(0, 16, "STM32F405 + AD9834", OLED_6X8);
    OLED_ShowString(0, 24, "IQ + FreeRTOS", OLED_6X8);
    OLED_ShowString(0, 40, "K1/K2/K3/K4: back", OLED_6X8);
}

static void App_Display_HandleCmd(KeyCmd_t cmd)
{
    if (cmd == KEY_CMD_NONE) {
        return;
    }

    switch (s_screen) {
    case SCREEN_MEASURE:
        if (cmd == KEY_CMD_KEY1_SHORT) {
            s_menu_index = 0u;
            s_screen = SCREEN_MENU;
        } else if (cmd == KEY_CMD_KEY4_SHORT) {
            s_freq_index = (uint8_t)((s_freq_index + 1u) % (sizeof(kFreqList) / sizeof(kFreqList[0])));
            App_Measure_SetFreq(kFreqList[s_freq_index]);
        } else if (cmd == KEY_CMD_KEY2_SHORT) {
            s_disp_mode = (s_disp_mode == DISP_MODE_NORMAL) ? DISP_MODE_DETAIL : DISP_MODE_NORMAL;
        }
        break;

    case SCREEN_MENU:
        if (cmd == KEY_CMD_KEY2_SHORT) {
            if (s_menu_index > 0u) {
                s_menu_index--;
            }
        } else if (cmd == KEY_CMD_KEY3_SHORT) {
            if (s_menu_index < 3u) {
                s_menu_index++;
            }
        } else if (cmd == KEY_CMD_KEY1_SHORT) {
            if (s_menu_index == 0u) {
                s_screen = SCREEN_SET_FREQ;
            } else if (s_menu_index == 1u) {
                s_screen = SCREEN_SET_AMP;
            } else if (s_menu_index == 2u) {
                s_screen = SCREEN_SET_DISPMODE;
            } else {
                s_screen = SCREEN_ABOUT;
            }
        } else if (cmd == KEY_CMD_KEY1_LONG) {
            s_screen = SCREEN_MEASURE;
        }
        break;

    case SCREEN_SET_FREQ:
        if (cmd == KEY_CMD_KEY2_SHORT) {
            if (s_freq_index > 0u) {
                s_freq_index--;
            }
        } else if (cmd == KEY_CMD_KEY3_SHORT) {
            if (s_freq_index < ((sizeof(kFreqList) / sizeof(kFreqList[0])) - 1u)) {
                s_freq_index++;
            }
        } else if (cmd == KEY_CMD_KEY1_SHORT) {
            App_Measure_SetFreq(kFreqList[s_freq_index]);
            s_screen = SCREEN_MENU;
        } else if (cmd == KEY_CMD_KEY1_LONG) {
            s_screen = SCREEN_MENU;
        }
        break;

    case SCREEN_SET_AMP:
        if (cmd == KEY_CMD_KEY2_SHORT) {
            if (s_amp_index > 0u) {
                s_amp_index--;
            }
        } else if (cmd == KEY_CMD_KEY3_SHORT) {
            if (s_amp_index < ((sizeof(kAmpStr) / sizeof(kAmpStr[0])) - 1u)) {
                s_amp_index++;
            }
        } else if (cmd == KEY_CMD_KEY1_SHORT) {
            BSP_AFE_SetDDSAmp((AFE_DDS_Amp_t)s_amp_index);
            s_screen = SCREEN_MENU;
        } else if (cmd == KEY_CMD_KEY1_LONG) {
            s_screen = SCREEN_MENU;
        }
        break;

    case SCREEN_SET_DISPMODE:
        if ((cmd == KEY_CMD_KEY2_SHORT) || (cmd == KEY_CMD_KEY3_SHORT)) {
            s_disp_mode = (s_disp_mode == DISP_MODE_NORMAL) ? DISP_MODE_DETAIL : DISP_MODE_NORMAL;
        } else if ((cmd == KEY_CMD_KEY1_SHORT) || (cmd == KEY_CMD_KEY1_LONG)) {
            s_screen = SCREEN_MENU;
        }
        break;

    case SCREEN_ABOUT:
        s_screen = SCREEN_MENU;
        break;

    default:
        s_screen = SCREEN_MEASURE;
        break;
    }
}

void App_Display_Init(void)
{
    memset(&s_last_result, 0, sizeof(s_last_result));

    OLED_Init();
    OLED_Clear();
    OLED_ShowString(16, 8, "LCR Meter", OLED_8X16);
    OLED_ShowString(8, 32, "Init...", OLED_6X8);
    OLED_Update();
    osDelay(400u);

    s_screen = SCREEN_MEASURE;
    s_disp_mode = DISP_MODE_NORMAL;
    s_menu_index = 0u;
    s_freq_index = 1u;
    s_amp_index = 0u;

    App_Measure_SetFreq(kFreqList[s_freq_index]);
    BSP_AFE_SetDDSAmp((AFE_DDS_Amp_t)s_amp_index);

    s_bat_v = App_Measure_GetBatVoltage();
}

void App_Display_Run(void)
{
    uint8_t raw_cmd;
    MeasureResult_t current = s_last_result;

    while (osMessageQueueGet(Queue_CmdHandle, &raw_cmd, NULL, 0u) == osOK) {
        App_Display_HandleCmd((KeyCmd_t)raw_cmd);
    }

    s_bat_ticks++;
    if (s_bat_ticks >= 100u) {
        s_bat_ticks = 0u;
        s_bat_v = App_Measure_GetBatVoltage();
    }

    if (App_Measure_GetResult(&current) != 0u) {
        s_last_result = current;
    }

    OLED_Clear();

    switch (s_screen) {
    case SCREEN_MEASURE:
        App_Display_DrawMeasure(&s_last_result);
        break;
    case SCREEN_MENU:
        App_Display_DrawMenu();
        break;
    case SCREEN_SET_FREQ:
        App_Display_DrawSetFreq();
        break;
    case SCREEN_SET_AMP:
        App_Display_DrawSetAmp();
        break;
    case SCREEN_SET_DISPMODE:
        App_Display_DrawSetMode();
        break;
    case SCREEN_ABOUT:
        App_Display_DrawAbout();
        break;
    default:
        s_screen = SCREEN_MEASURE;
        break;
    }

    OLED_Update();
    osDelay(100u);
}
