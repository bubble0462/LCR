/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY1_Pin GPIO_PIN_13
#define KEY1_GPIO_Port GPIOC
#define SOGI_VOP_COMP_Pin GPIO_PIN_0
#define SOGI_VOP_COMP_GPIO_Port GPIOC
#define SOGI_VOP_COMP_EXTI_IRQn EXTI0_IRQn
#define PS_Pin GPIO_PIN_1
#define PS_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_2
#define KEY2_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_3
#define KEY3_GPIO_Port GPIOC
#define IR_S0_Pin GPIO_PIN_0
#define IR_S0_GPIO_Port GPIOA
#define IR_S1_Pin GPIO_PIN_1
#define IR_S1_GPIO_Port GPIOA
#define ADC_VBAT_Pin GPIO_PIN_2
#define ADC_VBAT_GPIO_Port GPIOA
#define ADC_OUT_Pin GPIO_PIN_3
#define ADC_OUT_GPIO_Port GPIOA
#define FSYNC_Pin GPIO_PIN_4
#define FSYNC_GPIO_Port GPIOA
#define SCLK_Pin GPIO_PIN_5
#define SCLK_GPIO_Port GPIOA
#define RESET_Pin GPIO_PIN_6
#define RESET_GPIO_Port GPIOA
#define SDATA_Pin GPIO_PIN_7
#define SDATA_GPIO_Port GPIOA
#define KEY4_Pin GPIO_PIN_4
#define KEY4_GPIO_Port GPIOC
#define IO1_Pin GPIO_PIN_5
#define IO1_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_2
#define LED3_GPIO_Port GPIOB
#define TX2_Pin GPIO_PIN_10
#define TX2_GPIO_Port GPIOB
#define RX2_Pin GPIO_PIN_11
#define RX2_GPIO_Port GPIOB
#define S0_FIL_52_Pin GPIO_PIN_12
#define S0_FIL_52_GPIO_Port GPIOB
#define S1_FIL_52_Pin GPIO_PIN_13
#define S1_FIL_52_GPIO_Port GPIOB
#define S1_G_52_Pin GPIO_PIN_14
#define S1_G_52_GPIO_Port GPIOB
#define S0_G_52_Pin GPIO_PIN_15
#define S0_G_52_GPIO_Port GPIOB
#define CHRG_Pin GPIO_PIN_6
#define CHRG_GPIO_Port GPIOC
#define STDBY_Pin GPIO_PIN_7
#define STDBY_GPIO_Port GPIOC
#define S1_DDS_52_Pin GPIO_PIN_8
#define S1_DDS_52_GPIO_Port GPIOC
#define S0_DDS_52_Pin GPIO_PIN_9
#define S0_DDS_52_GPIO_Port GPIOC
#define FS_Pin GPIO_PIN_8
#define FS_GPIO_Port GPIOA
#define TX1_Pin GPIO_PIN_9
#define TX1_GPIO_Port GPIOA
#define RX1_Pin GPIO_PIN_10
#define RX1_GPIO_Port GPIOA
#define VR_S1_Pin GPIO_PIN_11
#define VR_S1_GPIO_Port GPIOA
#define VR_S0_Pin GPIO_PIN_12
#define VR_S0_GPIO_Port GPIOA
#define EN__5V_Pin GPIO_PIN_15
#define EN__5V_GPIO_Port GPIOA
#define S1_IO_53_Pin GPIO_PIN_10
#define S1_IO_53_GPIO_Port GPIOC
#define S0_IO_53_Pin GPIO_PIN_11
#define S0_IO_53_GPIO_Port GPIOC
#define IO2_Pin GPIO_PIN_12
#define IO2_GPIO_Port GPIOC
#define EN_VOUT_Pin GPIO_PIN_2
#define EN_VOUT_GPIO_Port GPIOD
#define EN__3_3V_Pin GPIO_PIN_3
#define EN__3_3V_GPIO_Port GPIOB
#define EN__3_3VB4_Pin GPIO_PIN_4
#define EN__3_3VB4_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_5
#define BUZZER_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
#define ANGLE_SW90_Pin GPIO_PIN_8
#define ANGLE_SW90_GPIO_Port GPIOB
#define ANGLE_SW0_Pin GPIO_PIN_9
#define ANGLE_SW0_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
