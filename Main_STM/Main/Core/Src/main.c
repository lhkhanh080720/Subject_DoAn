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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mfrc522.h"
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "i2c-lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
unsigned char CardID[4];
const uint8_t MyID[][4] =  {{0xBB, 0x68, 0xAC, 0x1C},		//white card
							{0xF3, 0x8A, 0x50, 0x12},		//SV02
							{0xE3, 0x11, 0x89, 0xA3}};		//new white card
const uint8_t Num_Card = sizeof(MyID) / sizeof(MyID[0]);
struct LIST_ID
{
	unsigned char ID[4];
	uint8_t index;
	uint8_t data[16];
};
uint8_t defaultKeyA[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t data[16];
uint8_t serNum[4];
unsigned char numData[10];
unsigned char c;
uint8_t clear = 0;
uint8_t t = 0;
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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim11;
RTC_HandleTypeDef hrtc;
void delay_ms(uint16_t ms);
void Read_Block(void);
void display(uint8_t n);
void clear_numData(void);
uint8_t CompareData(void);
int _write(int file, char *ptr, int len)
{
	int i = 0;
	for(i = 0; i < len; i++)
	{
		ITM_SendChar((*ptr++));
	}
	return len;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

//	struct LIST_ID list[Num_Card];
//	for (int i = 0; i < Num_Card; i++)
//	{
//		for (int j = 0; j < 4; j++)
//		{
//			list[i].ID[j] = MyID[i][j];
//		}
////		list[i].index = 0;
//	}
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
  MX_RTC_Init();
  MX_TIM2_Init();
  MX_TIM11_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	HAL_TIM_Base_Start(&htim11);
//	RTC_TimeTypeDef sTime;
//	RTC_DateTypeDef sDate;

	lcd_clear();
	lcd_put_cur(0,0);
	lcd_send_string("RUNNING...");
	htim2.Instance->CCR2 = 30;
	htim2.Instance->CCR1 = 118;
	Buzzer_Announ(2000);
	clear_numData();
	MFRC522_Reset();
	MFRC522_AntennaOn();
	MFRC522_Out_Reset();
	MFRC522_Out_AntennaOn();
	htim2.Instance->CCR2 = 80;
	htim2.Instance->CCR1 = 65;
	lcd_clear();
	lcd_put_cur(0,0);
	lcd_send_string("IN: ");
	lcd_put_cur(1, 0);
	lcd_send_string("OUT: ");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//Check UID card
        if (MFRC522_Request(PICC_REQALL, serNum) == MI_OK)
		{
        	t = 1;
			if (MFRC522_Anticoll(serNum) != MI_OK) 										continue;
			if (MFRC522_Select(serNum) != MI_OK)										continue;
			if (MFRC522_AuthState(PICC_AUTHENT1A, 4, defaultKeyA, serNum) != MI_OK)		continue;
			if (MFRC522_Read(4, data) != MI_OK)											continue;
			if(data[0] != 48)
			{
				lcd_put_cur(0,4);
				lcd_send_string("Error");
				Buzzer_Announ(500);
				lcd_put_cur(0,0);
				lcd_send_string("IN:                ");
				continue;
			}
			else
			{
				HAL_UART_Transmit(&huart6, (uint8_t*) "I", 1, 300);
				while(HAL_UART_Receive(&huart6,(uint8_t*)  &c, 1, 300) != HAL_OK)
				{
					if(c != "D" && c != "E")
					{
						continue;
					}
					else
					{
						break;
					}
				}
				switch(c)
				{
					case 'E':
					{
						Buzzer_Announ(500);
						continue;
					}
					case 'D':
					{
						while(HAL_UART_Receive(&huart6, (uint8_t*)&numData, 10, 1000) != HAL_OK);
						MFRC522_Write(4, (uint8_t*) &numData);
						display(4);
						delay_ms(1000);
						lcd_put_cur(0,0);
						lcd_send_string("IN:                ");
						clear_numData();
						htim2.Instance->CCR2 = 30;
						delay_ms(700);
						continue;
					}
				}
			}

		}
		else if (MFRC522_Out_Request(PICC_REQALL, serNum) == MI_OK)
		{
			if (MFRC522_Out_Anticoll(serNum) != MI_OK) 										continue;
			if (MFRC522_Select_Out(serNum) != MI_OK)										continue;
			if (MFRC522_AuthState_Out(PICC_AUTHENT1A, 4, defaultKeyA, serNum) != MI_OK) 	continue;
			if (MFRC522_Read_Out(4, data) != MI_OK)											continue;
			if(data[0] == 48)
			{
				lcd_put_cur(1,5);
				lcd_send_string("Error");
				Buzzer_Announ(500);
				lcd_put_cur(1,0);
				lcd_send_string("OUT:                ");
				continue;
			}
			else
			{
				HAL_UART_Transmit(&huart6, (uint8_t*)"O", 1, 300);
				while(HAL_UART_Receive(&huart6,(uint8_t*)  &c, 1, 300) != HAL_OK)
				{
					if(c != "D" && c != "E")
					{
						continue;
					}
					else
					{
						break;
					}
				}
				switch(c)
				{
					case 'E':
					{
						Buzzer_Announ(500);
						continue;
					}
					case 'D':
					{
						while(HAL_UART_Receive(&huart6, (uint8_t*)&numData, 10, 1000) != HAL_OK);
						if(CompareData() == MI_OK)
						{
							MFRC522_Write_Out(4, (uint8_t*) "0000000000");
							display(5);
							delay_ms(1000);
							lcd_put_cur(1,0);
							lcd_send_string("OUT:              ");
							clear_numData();
							htim2.Instance->CCR1 = 118;
							delay_ms(700);
						}
						else
						{
							lcd_put_cur(1,5);
							lcd_send_string("Error");
							Buzzer_Announ(500);
							lcd_put_cur(1,0);
							lcd_send_string("OUT:                ");
						}
						continue;
					}
				}
			}
		}
		while(clear)
		{
			if (MFRC522_Request(PICC_REQALL, serNum) == MI_OK)
			{
				lcd_clear();
				lcd_put_cur(0, 0);
				lcd_send_string("----CLEAR----");
				if (MFRC522_Anticoll(serNum) != MI_OK) 										continue;
				if (MFRC522_Select(serNum) != MI_OK)										continue;
				if (MFRC522_AuthState(PICC_AUTHENT1A, 4, defaultKeyA, serNum) != MI_OK)		continue;
				if (MFRC522_Read(4, data) != MI_OK)											continue;
				while(MFRC522_Write(4, (uint8_t*) "0000000000") != MI_OK);
				lcd_clear();
				lcd_put_cur(0,0);
				lcd_send_string("IN: ");
				lcd_put_cur(1, 0);
				lcd_send_string("OUT: ");
				clear = 0;
			}
		}

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/************************************************************************
 Internal CLock = 72MHz = 72 000 000 Hz
 Ftimer = 72 000 000 / Prescaler(7200) = 10 000
 Ttimer = 1/Ftimer = 10^-4 s = 0.1 ms
 => Time ngat tran (ms) = 1ms => Counter Period = 1/0.1 = 10
 ************************************************************************/
void delay_ms(uint16_t ms)
{
	__HAL_TIM_SET_COUNTER(&htim11, 0);
	while(__HAL_TIM_GET_COUNTER(&htim11) < (ms * 10) );
}

void Buzzer_Announ(uint32_t time)
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
	delay_ms(time);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin) {
	case BUTTON_OUT_Pin: {
//		htim2.Instance->CCR1 = 118;
		clear = 1;
		break;
	}
	case BUTTON_IN_Pin: {
		htim2.Instance->CCR2 = 30;
		break;
	}
	case SENSOR_OUT_Pin: {
		htim2.Instance->CCR1 = 65;
		break;
	}
	case SENSOR_IN_Pin: {
		htim2.Instance->CCR2 = 80;
		break;
	}
	}
}

void display(uint8_t n) //IN --> n = 4; OUT --> n = 5;
{
	uint8_t x;
	if((char)numData[9] == '-')
		x = 9;
	else
		x = 10;
	for(int j = 0; j < x; j++)
	{
		lcd_put_cur((n - 4), j + n);
		lcd_send_data((char)numData[j]);
	}
}

uint8_t CompareData()
{
	uint8_t x;
	if((char)numData[9] == '-')
		x = 9;
	else
		x = 10;
	for (int i = 0; i < x; i++) {
		if (data[i] != numData[i])
		{
			return MI_ERR;
		}
	}
	return MI_OK;
}

void clear_numData()
{
	for(int i = 0; i < 10; i++)
	{
		numData[i] = 0;
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

