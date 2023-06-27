/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t readCplt = 0;
volatile uint8_t writeCplt = 0;
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	// https://community.st.com/t5/stm32cubemx-mcu/why-hal-dma-functions-are-not-working/td-p/400261
	// Add HAL_DMA_Abort_IT(hi2c->hdmarx) at interrupt, Otherwise dmaxferstatus = HAL_DMA_Start_IT() in HAL_I2C_Mem_Read_DMA will return busy
	if (hi2c->Instance == hi2c1.Instance) {
		HAL_DMA_Abort_IT(hi2c->hdmarx);

	}
	readCplt = 1;
}
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	// https://community.st.com/t5/stm32cubemx-mcu/why-hal-dma-functions-are-not-working/td-p/400261
	// Add HAL_DMA_Abort_IT(hi2c->hdmatx) at interrupt, Otherwise dmaxferstatus = HAL_DMA_Start_IT() in HAL_I2C_Mem_Write_DMA will return busy
	if (hi2c->Instance == hi2c1.Instance) {
		HAL_DMA_Abort_IT(hi2c->hdmatx);

	}
	writeCplt = 1;
}
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

	uint8_t rxData[16];
	uint8_t txData[16];
//	uint8_t address = 00;

	//HAL_I2C_Master_Transmit_DMA(&hi2c1, (uint16_t)(address<<0x01u), config, sizeof(config));
	//HAL_I2C_Master_Receive_DMA(&hi2c1, (uint16_t)((address<<0x01u)|0x01u), rxData, 16);

	volatile HAL_StatusTypeDef halStatus;
	// erase AT24C02
	memset(txData, 00, 16);
	HAL_I2C_Mem_Write_DMA(&hi2c1, 0xa0, 0, I2C_MEMADD_SIZE_8BIT, txData, 16);
	HAL_Delay(1000);
	// Read 16 bytes
	readCplt = 0;
	halStatus = HAL_I2C_Mem_Read_DMA(&hi2c1, 0xA0, 0, I2C_MEMADD_SIZE_8BIT,
			rxData, 16);
	HAL_Delay(1000);

	// Write two bytes @ address 0 to AT24C02
	writeCplt = 0;
	txData[0] = 0xAA;
	txData[1] = 0xBB;
	HAL_I2C_Mem_Write_DMA(&hi2c1, 0xa0, 0, I2C_MEMADD_SIZE_8BIT, txData, 2);
	HAL_Delay(1000);
	// Read 16 bytes
	readCplt = 0;
	halStatus = HAL_I2C_Mem_Read_DMA(&hi2c1, 0xA0, 0, I2C_MEMADD_SIZE_8BIT,
			rxData, 16);
	HAL_Delay(1000);

	// Write two bytes @ address 2 to AT24C02
	writeCplt = 0;
	txData[0] = 0xCC;
	txData[1] = 0xDD;
	HAL_I2C_Mem_Write_DMA(&hi2c1, 0xa0, 2, I2C_MEMADD_SIZE_8BIT, txData, 2);
	HAL_Delay(1000);
	// Read 16 bytes
	readCplt = 0;
	halStatus = HAL_I2C_Mem_Read_DMA(&hi2c1, 0xA0, 0, I2C_MEMADD_SIZE_8BIT,
			rxData, 16);
	HAL_Delay(1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
	while (1) {
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