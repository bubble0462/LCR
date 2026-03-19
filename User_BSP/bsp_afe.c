#include "bsp_afe.h"

#define SET_S0S1(val, s0_port, s0_pin, s1_port, s1_pin) \
    do { \
        HAL_GPIO_WritePin((s0_port), (s0_pin), ((val) & 0x01u) ? GPIO_PIN_SET : GPIO_PIN_RESET); \
        HAL_GPIO_WritePin((s1_port), (s1_pin), ((val) & 0x02u) ? GPIO_PIN_SET : GPIO_PIN_RESET); \
    } while (0)

static AFE_Rref_t s_rref = RREF_1K;
static AFE_PGA_Gain_t s_gain = PGA_GAIN_0;
static AFE_LPF_Freq_t s_lpf = LPF_1KHZ;
static AFE_DDS_Amp_t s_amp = DDS_AMP_0V3;

void BSP_AFE_Init(void)
{
    BSP_AFE_SetRref(RREF_1K);
    BSP_AFE_SetPGAGain(PGA_GAIN_0);
    BSP_AFE_SetLPF(LPF_1KHZ);
    BSP_AFE_SetMeasureChannel(SIG_CHANNEL_UX, PHASE_0DEG);
    BSP_AFE_SetDDSAmp(DDS_AMP_0V3);
}

void BSP_AFE_SetRref(AFE_Rref_t rref)
{
    SET_S0S1((uint8_t)rref, VR_S0_GPIO_Port, VR_S0_Pin, VR_S1_GPIO_Port, VR_S1_Pin);
    s_rref = rref;
}

AFE_Rref_t BSP_AFE_GetRref(void)
{
    return s_rref;
}

float BSP_AFE_GetRrefValue(void)
{
    switch (s_rref) {
    case RREF_20R:
        return RREF_VALUE_20R;
    case RREF_1K:
        return RREF_VALUE_1K;
    case RREF_10K:
        return RREF_VALUE_10K;
    case RREF_100K:
        return RREF_VALUE_100K;
    default:
        return RREF_VALUE_1K;
    }
}

void BSP_AFE_SetPGAGain(AFE_PGA_Gain_t gain)
{
    SET_S0S1((uint8_t)gain, S0_G_52_GPIO_Port, S0_G_52_Pin, S1_G_52_GPIO_Port, S1_G_52_Pin);
    s_gain = gain;
}

AFE_PGA_Gain_t BSP_AFE_GetPGAGain(void)
{
    return s_gain;
}

void BSP_AFE_SetLPF(AFE_LPF_Freq_t freq)
{
    SET_S0S1((uint8_t)freq, S0_FIL_52_GPIO_Port, S0_FIL_52_Pin, S1_FIL_52_GPIO_Port, S1_FIL_52_Pin);
    s_lpf = freq;
}

AFE_LPF_Freq_t BSP_AFE_GetLPF(void)
{
    return s_lpf;
}

void BSP_AFE_SetSigChannel(AFE_SigChannel_t ch)
{
    HAL_GPIO_WritePin(S0_IO_53_GPIO_Port, S0_IO_53_Pin, (ch == SIG_CHANNEL_UR) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void BSP_AFE_SetPhase(AFE_Phase_t phase)
{
    HAL_GPIO_WritePin(S1_IO_53_GPIO_Port, S1_IO_53_Pin, (phase == PHASE_90DEG) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void BSP_AFE_SetMeasureChannel(AFE_SigChannel_t ch, AFE_Phase_t phase)
{
    BSP_AFE_SetSigChannel(ch);
    BSP_AFE_SetPhase(phase);
}

void BSP_AFE_SetDDSAmp(AFE_DDS_Amp_t amp)
{
    SET_S0S1((uint8_t)amp, S0_DDS_52_GPIO_Port, S0_DDS_52_Pin, S1_DDS_52_GPIO_Port, S1_DDS_52_Pin);
    s_amp = amp;
}

AFE_DDS_Amp_t BSP_AFE_GetDDSAmp(void)
{
    return s_amp;
}
