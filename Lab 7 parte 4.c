/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  * LAB 7 CAMILA ARISTIZABAL
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FC_MIN_V       0.5f
#define FC_MAX_V       2.5f

#define SPO2_MIN_V     0.5f
#define SPO2_MAX_V     2.5f

#define PRESION_MIN_V  0.5f
#define PRESION_MAX_V  2.5f

#define LED_FC_PIN       GPIO_PIN_5
#define LED_SPO2_PIN     GPIO_PIN_6
#define LED_PRESION_PIN  GPIO_PIN_7

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define NUM_SENSORES 3
#define VREF_ADC     3.3f
#define ADC_MAX      4095.0f

volatile uint16_t adc_values[NUM_SENSORES] = {0};

volatile float voltaje_1 = 0.0f;
volatile float voltaje_2 = 0.0f;
volatile float voltaje_3 = 0.0f;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);
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
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  if (HAL_ADC_Start_DMA(&hadc1,
                        (uint32_t *)adc_values,
                        NUM_SENSORES) != HAL_OK)
  {
      Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  /* Convertir las lecturas ADC a voltajes */
	     voltaje_1 = ((float)adc_values[0] * VREF_ADC) / ADC_MAX;
	     voltaje_2 = ((float)adc_values[1] * VREF_ADC) / ADC_MAX;
	     voltaje_3 = ((float)adc_values[2] * VREF_ADC) / ADC_MAX;

	     /* Preparar el mensaje en la terminal*/
	     char tx_buffer[120];

	     unsigned int v1_mV = ((unsigned int)adc_values[0] * 3300U) / 4095U;
	     unsigned int v2_mV = ((unsigned int)adc_values[1] * 3300U) / 4095U;
	     unsigned int v3_mV = ((unsigned int)adc_values[2] * 3300U) / 4095U;

	     int len = snprintf(tx_buffer, sizeof(tx_buffer),
	                        "ADC1: %4u | ADC2: %4u | ADC3: %4u"
	                        " || V1: %u mV | V2: %u mV | V3: %u mV\r\n",
	                        (unsigned int)adc_values[0],
	                        (unsigned int)adc_values[1],
	                        (unsigned int)adc_values[2],
	                        v1_mV, v2_mV, v3_mV);

	     if (len > 0 && len < (int)sizeof(tx_buffer))
	     {
	         HAL_UART_Transmit(&huart2,
	                           (uint8_t *)tx_buffer,
	                           (uint16_t)len,
	                           100);
	     }

	     HAL_Delay(200);

	     // Se utilizaron las siguientes variables para las leds: Frecuencia cardiaca, Oxigenación y presión arterial


	     /* Evaluar alarma de frecuencia cardiaca */
	     if (voltaje_1 < FC_MIN_V || voltaje_1 > FC_MAX_V)
	     {
	         HAL_GPIO_WritePin(GPIOA, LED_FC_PIN, GPIO_PIN_SET);
	     }
	     else
	     {
	         HAL_GPIO_WritePin(GPIOA, LED_FC_PIN, GPIO_PIN_RESET);
	     }

	     /* Evaluar alarma de oxigenacion */
	     if (voltaje_2 < SPO2_MIN_V || voltaje_2 > SPO2_MAX_V)
	     {
	         HAL_GPIO_WritePin(GPIOA, LED_SPO2_PIN, GPIO_PIN_SET);
	     }
	     else
	     {
	         HAL_GPIO_WritePin(GPIOA, LED_SPO2_PIN, GPIO_PIN_RESET);
	     }

	     /* Evaluar alarma de presion arterial */
	     if (voltaje_3 < PRESION_MIN_V || voltaje_3 > PRESION_MAX_V)
	     {
	         HAL_GPIO_WritePin(GPIOA, LED_PRESION_PIN, GPIO_PIN_SET);
	     }
	     else
	     {
	         HAL_GPIO_WritePin(GPIOA, LED_PRESION_PIN, GPIO_PIN_RESET);
	     }

  /* USER CODE END 3 */
  }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
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

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_InjectionConfTypeDef sConfigInjected = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 3;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_0;
  sConfigInjected.InjectedRank = 1;
  sConfigInjected.InjectedNbrOfConversion = 1;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_84CYCLES;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_NONE;
  sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.InjectedOffset = 0;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	    /* Habilitar reloj de GPIOA */
	    __HAL_RCC_GPIOA_CLK_ENABLE();

	    /* Apagar los LED al iniciar */
	    HAL_GPIO_WritePin(GPIOA,
	                      GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,
	                      GPIO_PIN_RESET);

	    /* Configurar los tres pines como salidas */
	    GPIO_InitStruct.Pin = GPIO_PIN_5 |
	                          GPIO_PIN_6 |
	                          GPIO_PIN_7;

	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
