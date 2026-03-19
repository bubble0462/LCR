#ifndef APP_MEASURE_H
#define APP_MEASURE_H

#include <stdint.h>
#include "cmsis_os.h"
#include "system_data.h"

#define ADC_DMA_BUF_LEN 16u

extern uint16_t g_adc1_dma_buf[ADC_DMA_BUF_LEN];

void App_Measure_Init(void);
void App_Measure_Run(void);

uint8_t App_Measure_GetResult(MeasureResult_t *out);

void App_Measure_SetFreq(uint32_t freq_hz);
uint32_t App_Measure_GetFreq(void);

float App_Measure_GetBatVoltage(void);

#endif
