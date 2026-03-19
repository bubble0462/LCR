/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_display.h"
#include "app_key.h"
#include "app_measure.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_Measure */
osThreadId_t Task_MeasureHandle;
const osThreadAttr_t Task_Measure_attributes = {
  .name = "Task_Measure",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for Task_Display */
osThreadId_t Task_DisplayHandle;
const osThreadAttr_t Task_Display_attributes = {
  .name = "Task_Display",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_Key */
osThreadId_t Task_KeyHandle;
const osThreadAttr_t Task_Key_attributes = {
  .name = "Task_Key",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Queue_Cmd */
osMessageQueueId_t Queue_CmdHandle;
const osMessageQueueAttr_t Queue_Cmd_attributes = {
  .name = "Queue_Cmd"
};
/* Definitions for Mutex_Result */
osMutexId_t Mutex_ResultHandle;
const osMutexAttr_t Mutex_Result_attributes = {
  .name = "Mutex_Result"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartMeasureTask(void *argument);
void StartDisplayTask(void *argument);
void StartKeyTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of Mutex_Result */
  Mutex_ResultHandle = osMutexNew(&Mutex_Result_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of Queue_Cmd */
  Queue_CmdHandle = osMessageQueueNew (16, sizeof(uint8_t), &Queue_Cmd_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_Measure */
  Task_MeasureHandle = osThreadNew(StartMeasureTask, NULL, &Task_Measure_attributes);

  /* creation of Task_Display */
  Task_DisplayHandle = osThreadNew(StartDisplayTask, NULL, &Task_Display_attributes);

  /* creation of Task_Key */
  Task_KeyHandle = osThreadNew(StartKeyTask, NULL, &Task_Key_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartMeasureTask */
/**
  * @brief  Function implementing the Task_Measure thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMeasureTask */
void StartMeasureTask(void *argument)
{
  /* USER CODE BEGIN StartMeasureTask */
  App_Measure_Init();
  /* Infinite loop */
  for(;;)
  {
    App_Measure_Run();
  }
  /* USER CODE END StartMeasureTask */
}

/* USER CODE BEGIN Header_StartDisplayTask */
/**
* @brief Function implementing the Task_Display thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void *argument)
{
  /* USER CODE BEGIN StartDisplayTask */
  App_Display_Init();
  /* Infinite loop */
  for(;;)
  {
    App_Display_Run();
  }
  /* USER CODE END StartDisplayTask */
}

/* USER CODE BEGIN Header_StartKeyTask */
/**
* @brief Function implementing the Task_Key thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeyTask */
void StartKeyTask(void *argument)
{
  /* USER CODE BEGIN StartKeyTask */
  App_Key_Init();
  /* Infinite loop */
  for(;;)
  {
    App_Key_Run();
  }
  /* USER CODE END StartKeyTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

