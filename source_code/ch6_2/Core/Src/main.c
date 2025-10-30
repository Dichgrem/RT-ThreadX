/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/*
 * �?1）LED1用于指示系统工作状�?�，上电或复位后，系统进行初始化，初始化完成后LED1间隔0.5s闪烁3次，随后进入正常运行状�??, LED1间隔1s闪烁�?
 * �?2）按键K1和K2通过外部中断的方式控制LED2和LED3的状态，按下时对应LED亮，松开时对应LED灭�??
 * �?3）�?�过USART1将LED2、LED3、K1和K2状�?�信息发送至串口调试助手，发送时间间隔为2s�?
 * �?4）利用串口调试助手发送控制指令，实现LED2和LED3的开关控制�??
 *
 *
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
 TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* 状态标志位 */
uint8_t flag_1s = 0;//1s定时标志
uint8_t flag_2s = 0;//2s定时标志
uint8_t num_tim6 = 0;//TIM6中断次数标志
uint8_t flag_rec_usart1 = 0;//UASRT1接收完成中断标志

/* USART收发数组 */
uint8_t msg_rec_usart1[9];//串口1接收数组
uint8_t msg_send_usart1[11] = {0x48, 0x41, 0x49, 0x54};//串口1发送数组
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART1_UART_Init(void);
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
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  /* LED1间隔0.5s闪烁3次 */
  for(int i=0; i<6; i++)
  {
	  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);//LED1状态改变
	  HAL_Delay(500);//延时0.5s
  }

  /* 启动定时器 */
  HAL_TIM_Base_Start_IT(&htim6);
  /* 中断方式启动串口接收 */
  HAL_UART_Receive_IT(&huart1, msg_rec_usart1, 9);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(flag_1s)
	  {
		  flag_1s = 0;
		  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);//LED1闪烁
	  }
	  if(flag_2s)
	  {
		  flag_2s = 0;
		  /* 判断LED2和LED3状态，改变msg_send_usart1中对应位 */
		  if(HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin) == GPIO_PIN_RESET)
		  {
			  msg_send_usart1[5] = 0x01;
		  }
		  else
		  {
			  msg_send_usart1[5] = 0x00;
		  }

		  if(HAL_GPIO_ReadPin(LED3_GPIO_Port, LED3_Pin) == GPIO_PIN_RESET)
		  {
			  msg_send_usart1[6] = 0x01;
		  }
		  else
		  {
			  msg_send_usart1[6] = 0x00;
		  }
		  HAL_UART_Transmit(&huart1, msg_send_usart1, 11, 0xfff);//串口发送状态信息
	  }
	  if(flag_rec_usart1)
	  {
		  flag_rec_usart1 = 0;
		  /* 判断协议头 */
		  if(msg_rec_usart1[0]==0x48 && msg_rec_usart1[1]==0x41 && msg_rec_usart1[2]==0x49 && msg_rec_usart1[3]==0x54)
		  {
			  if(msg_rec_usart1[5] == 0x01)//控制LED2状态
			  {
				  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);//点亮LED2
				  msg_rec_usart1[5] = 0x01;//LED2状态
			  }
			  else
			  {
				  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//点亮LED2
				  msg_rec_usart1[5] = 0x00;//LED2状态
			  }

			  if(msg_rec_usart1[6] == 0x01)//控制LED3状态
			  {
				  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);//点亮LED2
				  msg_rec_usart1[6] = 0x01;//LED3状态
			  }
			  else
			  {
				  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);//点亮LED2
				  msg_rec_usart1[6] = 0x00;//LED3状态
			  }
		  }
	  }
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 3;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 8000-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 10000-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_Pin|LED2_Pin|LED3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : K1_Pin K2_Pin */
  GPIO_InitStruct.Pin = K1_Pin|K2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
/* EXTI中断回调函数 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == K1_Pin)
	{
		/* 根据K1按下还是断开，控制LED2亮灭 */
		if(HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_SET)//按下
		{
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);//LED2亮
			msg_send_usart1[5] = 0x01;//LED2状态
			msg_send_usart1[7] = 0x01;//K1状态
		}
		else
		{
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);//LED2灭
			msg_send_usart1[5] = 0x00;//LED2状态
			msg_send_usart1[7] = 0x00;//K1状态
		}
	}
	else
	{
		/* 根据K2按下还是断开，控制LED3亮灭 */
		if(HAL_GPIO_ReadPin(K2_GPIO_Port, K2_Pin) == GPIO_PIN_SET)//按下
		{
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);//LED3亮
			msg_send_usart1[6] = 0x01;//LED3状态
			msg_send_usart1[8] = 0x01;//K2状态
		}
		else
		{
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);//LED3灭
			msg_send_usart1[6] = 0x00;//LED3状态
			msg_send_usart1[8] = 0x00;//K2状态
		}
	}
}

/* TIM中断回调函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		num_tim6++;
		flag_1s = 1;
		if(num_tim6 == 2)
		{
			flag_2s = 1;
			num_tim6 = 0;
		}
	}
}

/* USART中断回调函数 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		flag_rec_usart1 = 1;
		HAL_UART_Receive_IT(&huart1, msg_rec_usart1, 9);//再次启动中断接收
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
