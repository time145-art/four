/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */
 #include <stdio.h>
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
 #include <stdio.h>
 typedef struct{
  int16_t ch[10];
  uint8_t loss;
  uint8_t fail;
  uint32_t last_time;
 }RC_contral;

 volatile  RC_contral rc_data;
 uint8_t sbus_rx_buf[25];
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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
   rc_data.last_time=HAL_GetTick();
   HAL_UARTEx_ReceiveToIdle_DMA(&huart3,sbus_rx_buf,25);
  __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (HAL_GetTick() - rc_data.last_time > 100) {
    rc_data.fail=1;
    rc_data.loss=1;
    for (int i = 0; i < 10; i++) {
        rc_data.ch[i] = 0;
    }
}

HAL_Delay(10);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef*huart, uint16_t Size){
    if(huart == &huart3){
        if(Size == 25 && sbus_rx_buf[0] == 0x0F) {
            
            rc_data.ch[0] = (((int16_t)sbus_rx_buf[1]       | ((int16_t)sbus_rx_buf[2] << 8)) & 0x07FF) - 1024;
            rc_data.ch[1] = (((int16_t)sbus_rx_buf[2] >> 3  | ((int16_t)sbus_rx_buf[3] << 5)) & 0x07FF) - 1024;
            rc_data.ch[2] = (((int16_t)sbus_rx_buf[3] >> 6  | ((int16_t)sbus_rx_buf[4] << 2) | ((int16_t)sbus_rx_buf[5] << 10)) & 0x07FF) - 1024;
            rc_data.ch[3] = (((int16_t)sbus_rx_buf[5] >> 1  | ((int16_t)sbus_rx_buf[6] << 7)) & 0x07FF) - 1024;
            rc_data.ch[4] = (((int16_t)sbus_rx_buf[6] >> 4  | ((int16_t)sbus_rx_buf[7] << 4)) & 0x07FF) - 1024;
            rc_data.ch[5] = (((int16_t)sbus_rx_buf[7] >> 7  | ((int16_t)sbus_rx_buf[8] << 1) | ((int16_t)sbus_rx_buf[9] << 9)) & 0x07FF) - 1024;
            rc_data.ch[6] = (((int16_t)sbus_rx_buf[9] >> 2  | ((int16_t)sbus_rx_buf[10] << 6)) & 0x07FF) - 1024;
            rc_data.ch[7] = (((int16_t)sbus_rx_buf[10] >> 5 | ((int16_t)sbus_rx_buf[11] << 3)) & 0x07FF) - 1024;
            rc_data.ch[8] = (((int16_t)sbus_rx_buf[12]      | ((int16_t)sbus_rx_buf[13] << 8)) & 0x07FF) - 1024;
            rc_data.ch[9] = (((int16_t)sbus_rx_buf[13] >> 3 | ((int16_t)sbus_rx_buf[14] << 5)) & 0x07FF) - 1024;
            
            rc_data.fail = (sbus_rx_buf[23] & 0x08) ? 1 : 0;
            rc_data.loss = (sbus_rx_buf[23] & 0x04) ? 1 : 0;
            rc_data.last_time = HAL_GetTick();
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, sbus_rx_buf, 25);
        __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
