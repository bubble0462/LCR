#ifndef BSP_TIM4_H
#define BSP_TIM4_H

#include "main.h"
#include <stdint.h>

void BSP_TIM4_Init(void);
void BSP_TIM4_SetFreq(uint32_t freq_hz);
void BSP_TIM4_PhaseSync(void);
uint32_t BSP_TIM4_GetSyncCount(void);

#endif
