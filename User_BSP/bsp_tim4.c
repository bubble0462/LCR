#include "bsp_tim4.h"
#include "tim.h"
#include "stm32f4xx_hal.h"

typedef struct {
    uint32_t freq_hz;
    uint16_t arr;
    uint16_t ccr4;
    uint16_t ccr3;
} Tim4FreqParam_t;

static const Tim4FreqParam_t kFreqTable[] = {
    {100u, 8399u, 4200u, 2100u},
    {1000u, 839u, 420u, 210u},
    {10000u, 83u, 42u, 21u},
    {100000u, 7u, 4u, 2u},
};

static uint32_t s_current_freq_hz = 1000u;
static uint8_t s_sync_enable = 1u;
static volatile uint32_t s_sync_count = 0u;

extern TIM_HandleTypeDef htim4;

static const Tim4FreqParam_t *BSP_TIM4_FindParam(uint32_t freq_hz)
{
    uint32_t i;

    for (i = 0u; i < (sizeof(kFreqTable) / sizeof(kFreqTable[0])); i++) {
        if (kFreqTable[i].freq_hz == freq_hz) {
            return &kFreqTable[i];
        }
    }

    return &kFreqTable[1];
}

static void BSP_TIM4_ApplyParam(const Tim4FreqParam_t *param)
{
    __HAL_TIM_SET_AUTORELOAD(&htim4, param->arr);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, param->ccr4);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, param->ccr3);
    __HAL_TIM_SET_COUNTER(&htim4, 0u);
}

void BSP_TIM4_Init(void)
{
    const Tim4FreqParam_t *param = BSP_TIM4_FindParam(1000u);

    HAL_TIM_OC_Stop(&htim4, TIM_CHANNEL_3);
    HAL_TIM_OC_Stop(&htim4, TIM_CHANNEL_4);

    BSP_TIM4_ApplyParam(param);

    s_current_freq_hz = param->freq_hz;
    s_sync_enable = 1u;

    HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_4);
}

void BSP_TIM4_SetFreq(uint32_t freq_hz)
{
    const Tim4FreqParam_t *param = BSP_TIM4_FindParam(freq_hz);

    if (s_current_freq_hz == param->freq_hz) {
        return;
    }

    HAL_TIM_OC_Stop(&htim4, TIM_CHANNEL_3);
    HAL_TIM_OC_Stop(&htim4, TIM_CHANNEL_4);

    BSP_TIM4_ApplyParam(param);

    s_current_freq_hz = param->freq_hz;
    s_sync_enable = (param->freq_hz <= 10000u) ? 1u : 0u;

    HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_4);
}

void BSP_TIM4_PhaseSync(void)
{
    if (s_sync_enable == 0u) {
        return;
    }

    TIM4->CNT = 0u;
    s_sync_count++;
}

uint32_t BSP_TIM4_GetSyncCount(void)
{
    return s_sync_count;
}
