#ifndef BSP_AFE_H
#define BSP_AFE_H

#include "main.h"
#include <stdint.h>

typedef enum {
    RREF_20R = 0,
    RREF_1K = 1,
    RREF_10K = 2,
    RREF_100K = 3,
} AFE_Rref_t;

typedef enum {
    PGA_GAIN_0 = 0,
    PGA_GAIN_1 = 1,
    PGA_GAIN_2 = 2,
    PGA_GAIN_3 = 3,
} AFE_PGA_Gain_t;

typedef enum {
    LPF_100HZ = 0,
    LPF_1KHZ = 1,
    LPF_10KHZ = 2,
    LPF_100KHZ = 3,
} AFE_LPF_Freq_t;

typedef enum {
    SIG_CHANNEL_UX = 0,
    SIG_CHANNEL_UR = 1,
} AFE_SigChannel_t;

typedef enum {
    PHASE_0DEG = 0,
    PHASE_90DEG = 1,
} AFE_Phase_t;

typedef enum {
    DDS_AMP_0V3 = 0,
    DDS_AMP_0V6 = 1,
    DDS_AMP_1V0 = 2,
    DDS_AMP_NC = 3,
} AFE_DDS_Amp_t;

#define RREF_VALUE_20R   20.0f
#define RREF_VALUE_1K    1000.0f
#define RREF_VALUE_10K   10000.0f
#define RREF_VALUE_100K  100000.0f

void BSP_AFE_Init(void);

void BSP_AFE_SetRref(AFE_Rref_t rref);
AFE_Rref_t BSP_AFE_GetRref(void);
float BSP_AFE_GetRrefValue(void);

void BSP_AFE_SetPGAGain(AFE_PGA_Gain_t gain);
AFE_PGA_Gain_t BSP_AFE_GetPGAGain(void);

void BSP_AFE_SetLPF(AFE_LPF_Freq_t freq);
AFE_LPF_Freq_t BSP_AFE_GetLPF(void);

void BSP_AFE_SetSigChannel(AFE_SigChannel_t ch);
void BSP_AFE_SetPhase(AFE_Phase_t phase);
void BSP_AFE_SetMeasureChannel(AFE_SigChannel_t ch, AFE_Phase_t phase);

void BSP_AFE_SetDDSAmp(AFE_DDS_Amp_t amp);
AFE_DDS_Amp_t BSP_AFE_GetDDSAmp(void);

#endif
