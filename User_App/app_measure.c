#include "app_measure.h"

#include <math.h>
#include <string.h>

#include "adc.h"
#include "9834.h"
#include "bsp_afe.h"
#include "bsp_tim4.h"
#include "cmsis_os.h"

#define TASK_PERIOD_MS 10u

#define WAIT_V1_TICKS 30u
#define WAIT_V2_TICKS 30u
#define WAIT_V3_TICKS 30u
#define WAIT_I_TICKS  30u

#define RANGE_UP_THRESH 0.85f
#define RANGE_DOWN_THRESH 0.10f

#define ADC_FULL_SCALE 4095.0f
#define PI_F 3.14159265359f

#define VBAT_DIV_RATIO (110000.0f / (1000000.0f + 110000.0f))
#define ADC_VREF 3.3f

typedef enum {
    STATE_INIT = 0,
    STATE_SAMPLE_V1,
    STATE_SAMPLE_V2,
    STATE_SAMPLE_V3,
    STATE_SAMPLE_I,
    STATE_CALCULATE,
} MeasureState_t;

typedef struct {
    float y1;
    float y2;
    float alpha_slow;
    float alpha_fast;
    float threshold;
} DualAlphaFilter_t;

extern osMutexId_t Mutex_ResultHandle;

uint16_t g_adc1_dma_buf[ADC_DMA_BUF_LEN];

static MeasureState_t s_state = STATE_INIT;
static uint32_t s_wait_ticks = 0u;

static float s_iq_data[4];
static DualAlphaFilter_t s_filter[4];

static MeasureResult_t s_result;
static uint32_t s_freq_hz = 1000u;
static uint8_t s_adc_dma_ok = 0u;

static void App_Measure_ResetStateMachine(void)
{
    s_state = STATE_INIT;
    s_wait_ticks = 0u;
}

static void App_Measure_FilterInit(DualAlphaFilter_t *f, float alpha_slow, float alpha_fast, float threshold)
{
    f->y1 = 0.0f;
    f->y2 = 0.0f;
    f->alpha_slow = alpha_slow;
    f->alpha_fast = alpha_fast;
    f->threshold = threshold;
}

static float App_Measure_FilterUpdate(DualAlphaFilter_t *f, float raw)
{
    float alpha = f->alpha_slow;

    if (fabsf(raw - f->y1) > f->threshold) {
        alpha = f->alpha_fast;
    }

    f->y1 += alpha * (raw - f->y1);
    f->y2 += alpha * (f->y1 - f->y2);

    return f->y2;
}

static void App_Measure_FilterResetAll(float value)
{
    uint32_t i;

    for (i = 0u; i < 4u; i++) {
        s_filter[i].y1 = value;
        s_filter[i].y2 = value;
    }
}

static uint32_t App_Measure_NormalizeFreq(uint32_t freq_hz)
{
    static const uint32_t kFreqList[] = {100u, 1000u, 10000u, 100000u};
    uint32_t best = kFreqList[0];
    uint32_t best_diff = (freq_hz > best) ? (freq_hz - best) : (best - freq_hz);
    uint32_t i;

    for (i = 1u; i < (sizeof(kFreqList) / sizeof(kFreqList[0])); i++) {
        uint32_t candidate = kFreqList[i];
        uint32_t diff = (freq_hz > candidate) ? (freq_hz - candidate) : (candidate - freq_hz);
        if (diff < best_diff) {
            best = candidate;
            best_diff = diff;
        }
    }

    return best;
}

static AFE_LPF_Freq_t App_Measure_FreqToLpf(uint32_t freq_hz)
{
    if (freq_hz <= 100u) {
        return LPF_100HZ;
    }
    if (freq_hz <= 1000u) {
        return LPF_1KHZ;
    }
    if (freq_hz <= 10000u) {
        return LPF_10KHZ;
    }
    return LPF_100KHZ;
}

static uint8_t App_Measure_PrepareAdcDma(void)
{
    ADC_ChannelConfTypeDef sConfig;

    (void)HAL_ADC_Stop_DMA(&hadc1);
    (void)HAL_ADC_Stop(&hadc1);

    if (HAL_ADC_DeInit(&hadc1) != HAL_OK) {
        return 0u;
    }

    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        return 0u;
    }

    memset(&sConfig, 0, sizeof(sConfig));
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return 0u;
    }

    memset(g_adc1_dma_buf, 0, sizeof(g_adc1_dma_buf));

    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)g_adc1_dma_buf, ADC_DMA_BUF_LEN) != HAL_OK) {
        return 0u;
    }

    return 1u;
}

static float App_Measure_ReadAdcPolling(void)
{
    float norm = 0.0f;

    if (HAL_ADC_Start(&hadc1) == HAL_OK) {
        if (HAL_ADC_PollForConversion(&hadc1, 3u) == HAL_OK) {
            uint16_t raw = (uint16_t)HAL_ADC_GetValue(&hadc1);
            norm = ((float)raw / ADC_FULL_SCALE) * 2.0f - 1.0f;
        }
        (void)HAL_ADC_Stop(&hadc1);
    }

    return norm;
}

static float App_Measure_AdcSample(void)
{
    if (s_adc_dma_ok != 0u) {
        uint32_t i;
        uint32_t sum = 0u;

        for (i = 0u; i < ADC_DMA_BUF_LEN; i++) {
            sum += g_adc1_dma_buf[i];
        }

        return (((float)sum / (float)ADC_DMA_BUF_LEN) / ADC_FULL_SCALE) * 2.0f - 1.0f;
    }

    return App_Measure_ReadAdcPolling();
}

static uint8_t App_Measure_AutoRange(void)
{
    float max_abs = 0.0f;
    uint32_t i;
    uint8_t changed = 0u;

    AFE_Rref_t rref = BSP_AFE_GetRref();
    AFE_PGA_Gain_t gain = BSP_AFE_GetPGAGain();

    for (i = 0u; i < 4u; i++) {
        float v = fabsf(s_iq_data[i]);
        if (v > max_abs) {
            max_abs = v;
        }
    }

    if (max_abs > RANGE_UP_THRESH) {
        if (gain > PGA_GAIN_0) {
            BSP_AFE_SetPGAGain((AFE_PGA_Gain_t)(gain - 1));
            changed = 1u;
        } else if (rref > RREF_20R) {
            BSP_AFE_SetRref((AFE_Rref_t)(rref - 1));
            changed = 1u;
        }
    } else if (max_abs < RANGE_DOWN_THRESH) {
        if (gain < PGA_GAIN_3) {
            BSP_AFE_SetPGAGain((AFE_PGA_Gain_t)(gain + 1));
            changed = 1u;
        } else if (rref < RREF_100K) {
            BSP_AFE_SetRref((AFE_Rref_t)(rref + 1));
            changed = 1u;
        }
    }

    if (changed != 0u) {
        App_Measure_FilterResetAll(0.0f);
        s_result.valid = 0u;
        App_Measure_ResetStateMachine();
    }

    return changed;
}

static void App_Measure_CalcImpedance(void)
{
    float a = s_iq_data[0];
    float b = s_iq_data[1];
    float c = s_iq_data[2];
    float d = s_iq_data[3];

    float rref = BSP_AFE_GetRrefValue();
    float denom = c * c + d * d;

    if (denom < 1e-9f) {
        s_result.valid = 0u;
        return;
    }

    s_result.re_ohm = (a * c + b * d) / denom * rref;
    s_result.im_ohm = (b * c - a * d) / denom * rref;
    s_result.z_mag_ohm = sqrtf(s_result.re_ohm * s_result.re_ohm + s_result.im_ohm * s_result.im_ohm);
    s_result.phase_deg = atan2f(s_result.im_ohm, s_result.re_ohm) * 180.0f / PI_F;

    s_result.freq_hz = (float)s_freq_hz;
    s_result.rref_range = BSP_AFE_GetRref();
    s_result.pga_gain = BSP_AFE_GetPGAGain();

    if (fabsf(s_result.phase_deg) < 45.0f) {
        s_result.comp_type = COMP_RESISTOR;
        s_result.primary_value = s_result.re_ohm;
        s_result.q_factor = (fabsf(s_result.re_ohm) > 1e-6f) ? fabsf(s_result.im_ohm / s_result.re_ohm) : 0.0f;
    } else if (s_result.im_ohm > 0.0f) {
        float omega = 2.0f * PI_F * (float)s_freq_hz;
        s_result.comp_type = COMP_INDUCTOR;
        s_result.primary_value = s_result.im_ohm / omega;
        s_result.q_factor = (fabsf(s_result.re_ohm) > 1e-6f) ? fabsf(s_result.im_ohm / s_result.re_ohm) : 0.0f;
    } else {
        float omega = 2.0f * PI_F * (float)s_freq_hz;
        s_result.comp_type = COMP_CAPACITOR;
        s_result.primary_value = (fabsf(s_result.im_ohm) > 1e-9f) ? (-1.0f / (omega * s_result.im_ohm)) : 0.0f;
        s_result.q_factor = (fabsf(s_result.re_ohm) > 1e-6f) ? fabsf(s_result.im_ohm / s_result.re_ohm) : 0.0f;
    }

    s_result.d_factor = (s_result.q_factor > 1e-6f) ? (1.0f / s_result.q_factor) : 0.0f;
    s_result.valid = 1u;
}

void App_Measure_Init(void)
{
    uint32_t i;

    memset(&s_result, 0, sizeof(s_result));
    memset(s_iq_data, 0, sizeof(s_iq_data));

    for (i = 0u; i < 4u; i++) {
        App_Measure_FilterInit(&s_filter[i], 0.05f, 0.5f, 0.1f);
    }

    BSP_AFE_Init();

    AD9834_Init();
    AD9834_Select_Wave(SINE_WAVE);

    BSP_TIM4_Init();

    s_adc_dma_ok = App_Measure_PrepareAdcDma();

    App_Measure_SetFreq(1000u);
    App_Measure_ResetStateMachine();
}

void App_Measure_SetFreq(uint32_t freq_hz)
{
    uint32_t new_freq = App_Measure_NormalizeFreq(freq_hz);

    s_freq_hz = new_freq;

    AD9834_Set_Freq(FREQ_0, (unsigned long)new_freq);
    BSP_TIM4_SetFreq(new_freq);
    BSP_AFE_SetLPF(App_Measure_FreqToLpf(new_freq));

    App_Measure_FilterResetAll(0.0f);
    s_result.valid = 0u;
    App_Measure_ResetStateMachine();
}

uint32_t App_Measure_GetFreq(void)
{
    return s_freq_hz;
}

uint8_t App_Measure_GetResult(MeasureResult_t *out)
{
    uint8_t ok = 0u;

    if (out == NULL) {
        return 0u;
    }

    if (osMutexAcquire(Mutex_ResultHandle, 20u) == osOK) {
        memcpy(out, &s_result, sizeof(MeasureResult_t));
        osMutexRelease(Mutex_ResultHandle);
        ok = 1u;
    }

    return ok;
}

float App_Measure_GetBatVoltage(void)
{
    float vbat = 0.0f;

    if (HAL_ADC_Start(&hadc2) == HAL_OK) {
        if (HAL_ADC_PollForConversion(&hadc2, 10u) == HAL_OK) {
            uint16_t raw = (uint16_t)HAL_ADC_GetValue(&hadc2);
            float v_pa2 = ((float)raw / ADC_FULL_SCALE) * ADC_VREF;
            vbat = v_pa2 / VBAT_DIV_RATIO;
        }
        (void)HAL_ADC_Stop(&hadc2);
    }

    return vbat;
}

void App_Measure_Run(void)
{
    s_wait_ticks++;

    switch (s_state) {
    case STATE_INIT:
        BSP_AFE_SetMeasureChannel(SIG_CHANNEL_UX, PHASE_0DEG);
        s_wait_ticks = 0u;
        s_state = STATE_SAMPLE_V1;
        break;

    case STATE_SAMPLE_V1:
        s_iq_data[0] = App_Measure_FilterUpdate(&s_filter[0], App_Measure_AdcSample());
        if (s_wait_ticks >= WAIT_V1_TICKS) {
            s_iq_data[0] = s_filter[0].y2;
            BSP_AFE_SetMeasureChannel(SIG_CHANNEL_UX, PHASE_90DEG);
            s_wait_ticks = 0u;
            s_state = STATE_SAMPLE_V2;
        }
        break;

    case STATE_SAMPLE_V2:
        s_iq_data[1] = App_Measure_FilterUpdate(&s_filter[1], App_Measure_AdcSample());
        if (s_wait_ticks >= WAIT_V2_TICKS) {
            s_iq_data[1] = s_filter[1].y2;
            BSP_AFE_SetMeasureChannel(SIG_CHANNEL_UR, PHASE_90DEG);
            s_wait_ticks = 0u;
            s_state = STATE_SAMPLE_V3;
        }
        break;

    case STATE_SAMPLE_V3:
        s_iq_data[3] = App_Measure_FilterUpdate(&s_filter[3], App_Measure_AdcSample());
        if (s_wait_ticks >= WAIT_V3_TICKS) {
            s_iq_data[3] = s_filter[3].y2;
            BSP_AFE_SetMeasureChannel(SIG_CHANNEL_UR, PHASE_0DEG);
            s_wait_ticks = 0u;
            s_state = STATE_SAMPLE_I;
        }
        break;

    case STATE_SAMPLE_I:
        s_iq_data[2] = App_Measure_FilterUpdate(&s_filter[2], App_Measure_AdcSample());
        if (s_wait_ticks >= WAIT_I_TICKS) {
            s_iq_data[2] = s_filter[2].y2;
            s_wait_ticks = 0u;
            s_state = STATE_CALCULATE;
        }
        break;

    case STATE_CALCULATE:
        if (App_Measure_AutoRange() == 0u) {
            if (osMutexAcquire(Mutex_ResultHandle, 10u) == osOK) {
                App_Measure_CalcImpedance();
                osMutexRelease(Mutex_ResultHandle);
            }
            App_Measure_ResetStateMachine();
        }
        break;

    default:
        App_Measure_ResetStateMachine();
        break;
    }

    osDelay(TASK_PERIOD_MS);
}
