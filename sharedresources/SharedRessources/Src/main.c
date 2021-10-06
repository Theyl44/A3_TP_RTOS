/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : 	.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "freertos.h"
#include "task.h"
#include "semphr.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define up 1
#define down 0

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
TaskHandle_t RedLedTaskHandle, GreenLedTaskHandle, OrangeLedTaskHandle;
uint32_t GreenLed, RedLed, BlueLed, OrangeLed = 0;
uint32_t sharedDataFlag = up;

SemaphoreHandle_t CriticalResourceSemaphoreHandleHandle;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void GreenLedTaskStart(void *argument);
void RedLedTaskStart(void *argument);
void OrangeLedTaskStart(void *argument);


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
 // HAL_Init();
	CriticalResourceSemaphoreHandleHandle = xSemaphoreCreateBinary();
	xSemaphoreGive(CriticalResourceSemaphoreHandleHandle);
	

  /* USER CODE BEGIN Init */
	xTaskCreate(GreenLedTaskStart, "GreenLedFlashTask", 200, NULL, 1, &GreenLedTaskHandle);
	
	xTaskCreate(RedLedTaskStart, "RedLedFlashTask", 200, NULL, 2, &RedLedTaskHandle);
	
	xTaskCreate(OrangeLedTaskStart, "OrangeLedFlashTask", 200, NULL, 2, &OrangeLedTaskHandle);


  /* USER CODE END Init */

  /* Configure the system clock */
  //SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  vTaskStartScheduler();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_GreenLedTaskStart */
/**
  * @brief  Function implementing the GreenLedTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_GreenLedTaskStart */
void GreenLedTaskStart(void *argument)
{
  /* USER CODE BEGIN 5 */
	uint32_t counter = 0;
	GreenLed = 0;
	
  /* Infinite loop */
  for(;;)
  {
		GreenLed = 1;
		// simulation access shared data
		//if (up == sharedDataFlag) {
		if (xSemaphoreTake(CriticalResourceSemaphoreHandleHandle, portMAX_DELAY) == pdTRUE){
			//taskENTER_CRITICAL();
			//xSemaphoreTake(CriticalResourceSemaphoreHandleHandle, portMAX_DELAY == pdTRUE);
			// shared data not used by another task
			sharedDataFlag = down;
			while (counter < 1500000) counter++;
			counter = 0;
			sharedDataFlag = up;
			//taskEXIT_CRITICAL();
			xSemaphoreGive(CriticalResourceSemaphoreHandleHandle);
		}
		else {
			// shared data used by another task
			BlueLed = 1;
		}
		vTaskDelay(200);
		GreenLed = 0;
    vTaskDelay(200);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_RedLedTaskStart */
/**
* @brief Function implementing the RedLedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RedLedTaskStart */
void RedLedTaskStart(void *argument)
{
 	/* USER CODE BEGIN RedLedTaskStart */
	uint32_t counter = 0;
	
  /* Infinite loop */
  for(;;)
  {
			RedLed = 1;
    // simulation access shared data
		//if (up == sharedDataFlag) {
		if(xSemaphoreTake(CriticalResourceSemaphoreHandleHandle, portMAX_DELAY) == pdTRUE){
			//taskENTER_CRITICAL();
			//xSemaphoreTake(CriticalResourceSemaphoreHandleHandle, portMAX_DELAY == pdTRUE);
			// shared data not used by another task
			sharedDataFlag = down;
			while (counter < 1500000) counter++;
			counter = 0;
			sharedDataFlag = up;
			//taskEXIT_CRITICAL();
			xSemaphoreGive(CriticalResourceSemaphoreHandleHandle);
		}
		else {
			// shared data used by another task
			BlueLed = 1;
		}
		vTaskDelay(550);
		RedLed = 0;
		vTaskDelay(550);
  }
  /* USER CODE END RedLedTaskStart */
}

void OrangeLedTaskStart(void *argument){
		for(;;){
			OrangeLed^=1;
			vTaskDelay(50);
		}
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
