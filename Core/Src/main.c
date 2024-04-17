/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "fonts.h"
#include <stdio.h> // funkcijai sprintf
#include "Statechart.h"
#include "Statechart_required.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define OLED_adress 0x78
#define EEPROM_adress 0xB0
#define Vref 3.3
#define N 4096
#define EEPROM_WriteBit 0
#define EEPROM_ReadBit 1
#define I2C_TIMEOUT 1000
#define NUM_ADC_CHANNELS 2
#define Min_threshold 1200 //1200
#define Max_threshold 3900
#define Data_3mV   0x40u  // B register (PB6)
#define Data_10mV  0x80u  // C register (PC7)
#define Data_30mV  0x400u // B register (PB10)
#define Data_100mV 0x10u  // B register (PB4)
#define Data_300mV 0x40u  // A register (PA6)
#define Data_1V    0x80u  // A register (PA7)
#define Data_3V    0x200u // A register (PA9)
#define Data_10V   0x100u // A register (PA8)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t TxBuffer[20];
uint8_t RxBuffer[20];
float coefficients_rms[9];
float coefficients_ampl[9];
float amplitude = 100.255689;
float rms = 1.2568;
uint16_t ADC_data[3] = {100, 250, 0};
uint16_t Sample_ampl = 0;
uint16_t Sample_RMS = 0;
int range_nr_ampl = 7;
int range_nr_rms = 7;
short Pins[9]={Data_3mV, Data_10mV, Data_30mV, Data_100mV, Data_300mV, Data_1V, Data_3V, Data_10V};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
Statechart sc_handle; // Statechart pointer

void ChangePinState(int nr, char function[4])
{
	if(strcmp(function,"ON")==0)
	{
		if(nr == 1)
			GPIOC->ODR&=~(Pins[nr]);
		else if((nr == 0) || (nr == 2) || (nr == 3))
			GPIOB->ODR&=~(Pins[nr]);
		else
			GPIOA->ODR&=~(Pins[nr]);
	}
	else if(strcmp(function,"OFF")==0)
	{
		if(nr == 1)
			GPIOC->ODR|=Pins[nr];
		else if((nr == 0) || (nr == 2) || (nr == 3))
			GPIOB->ODR|=Pins[nr];
		else
			GPIOA->ODR|=Pins[nr];
	}
}

void ReadCoefficients(void)
{
	for(int i=0; i<9; i++)
	{
		coefficients_rms[i] = 1;
		coefficients_ampl[i] = 1;
	}
}

void SendUARTData()
{
	sprintf((char *)TxBuffer,"%.2f/%.2f \n\r",amplitude, rms);
	HAL_UART_Transmit(&huart2, (uint8_t *)TxBuffer, sizeof(TxBuffer), 1);
	
}

void statechart_displayInfo(Statechart* handle)
	{
		//#ifndef DISABLE_DISPLAY
		static char string_display[30];
		// Display I2C sensor redings
		ssd1306_SetCursor(0, 15);
		sprintf(string_display,"RMS");
		//sprintf(string_display,"T0=%d.%dC",(int)AverageSensorI2C,(int)((AverageSensorI2C-
		ssd1306_WriteString(string_display,Font_11x18, White);
		// Display analog sensors readings
		ssd1306_SetCursor(40, 18); // ssd1306_SetCursor(0, 14);
		sprintf(string_display,"%.2f mV", amplitude);
		ssd1306_WriteString(string_display,Font_7x10, White);
		ssd1306_SetCursor(0, 37); // ssd1306_SetCursor(0, 14);
		sprintf(string_display,"A");
	  ssd1306_WriteString(string_display,Font_11x18, White);
		ssd1306_SetCursor(40, 40); // ssd1306_SetCursor(0, 14);
		sprintf(string_display,"%.2f mV", rms);
	  ssd1306_WriteString(string_display,Font_7x10, White);
		// Copy all data from local screenbuffer to the screen
		ssd1306_UpdateScreen(&hi2c3);
		//#endif
		__asm("NOP"); // for breakpoint
	}
	
void statechart_sendData(Statechart* handle)
	{
		SendUARTData();
	}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	{
		 if(htim==&htim6)
				statechart_raise_ev_GetSample(&sc_handle); //raise event TimerIntr in statechart
	}
	
void statechart_processData(Statechart* handle)
{
	amplitude = Sample_ampl*coefficients_ampl[range_nr_ampl]*3.3/4.096;
	rms = Sample_RMS*coefficients_rms[range_nr_rms]*3.3/4.096;
}

sc_integer statechart_readADCSample( Statechart* handle, const sc_integer channel)
{
	if (ADC_data[channel] > Max_threshold) // read from DMA memory when all channels are sampled
	{
		if(channel == 0)
		{
			ChangePinState(range_nr_ampl, "OFF");
			range_nr_ampl++;
			if(range_nr_ampl>7)
			{
				range_nr_ampl=7;
				ChangePinState(range_nr_ampl, "ON");
				range_nr_rms = range_nr_ampl;
				Sample_ampl = ADC_data[0];
				statechart_raise_ev_GoodSample(&sc_handle);
			}
			else
				ChangePinState(range_nr_ampl, "ON");
			range_nr_rms = range_nr_ampl;
		}
		else
		{
			ChangePinState(range_nr_rms, "OFF");
			range_nr_rms++;
			if(range_nr_rms>7)
			{
				range_nr_rms=7;
				ChangePinState(range_nr_rms, "ON");
				Sample_RMS = ADC_data[1];
				statechart_raise_ev_GoodSample(&sc_handle);
			}
			else
				ChangePinState(range_nr_rms, "ON");
		}
		statechart_raise_ev_ResetSample(&sc_handle);
	}
	else if((ADC_data[channel] < Min_threshold))
	{
		if(channel == 0)
		{
			ChangePinState(range_nr_ampl, "OFF");
			range_nr_ampl--;
			if(range_nr_ampl < 0)
			{
				range_nr_ampl=0;
				ChangePinState(range_nr_ampl, "ON");
				range_nr_rms = range_nr_ampl;
				Sample_ampl = ADC_data[0];
				statechart_raise_ev_GoodSample(&sc_handle);
			}
			ChangePinState(range_nr_ampl, "ON");
			range_nr_rms = range_nr_ampl;
		}
		else
		{
			ChangePinState(range_nr_rms, "OFF");
			range_nr_rms--;
			if(range_nr_rms < 0)
			{
				range_nr_rms=0;
				ChangePinState(range_nr_rms, "ON");
				Sample_RMS = ADC_data[1];
				statechart_raise_ev_GoodSample(&sc_handle);
			}
			ChangePinState(range_nr_rms, "ON");
		}
		statechart_raise_ev_ResetSample(&sc_handle);
	}
	else
	{
		if(channel == 0)
			Sample_ampl = ADC_data[0];
		else
			Sample_RMS = ADC_data[1];
		statechart_raise_ev_GoodSample(&sc_handle);
	}
 return 1;
}

sc_integer statechart_startConvADC( Statechart* handle, const sc_integer channel)
{
	if (HAL_ADC_Start_DMA(&hadc,(uint32_t *)ADC_data, 2) != HAL_OK)
	{
		return 0;
	}
	return 1;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	statechart_raise_ev_ADCSampleReady(&sc_handle); //raise event TimerIntr in statechart
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
  MX_TIM6_Init();
  MX_I2C3_Init();
  MX_USART4_UART_Init();
  MX_ADC_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	ssd1306_Init(&hi2c3);
  // Write data to local screenbuffer
	ssd1306_SetCursor(0,0);
	ssd1306_WriteString("STOP",Font_16x26, White);
	ssd1306_SetCursor(0, 12);

	// Copy all data from local screenbuffer to the screen
	ssd1306_UpdateScreen(&hi2c3);
	ReadCoefficients();
  //displayInfo();
	statechart_init(&sc_handle); //inicializuoti busenu automata
	statechart_enter(&sc_handle);
	if(HAL_TIM_Base_Start_IT(&htim6) != HAL_OK) //run TIM6 timer
	 {
			//ErrorHandler();
	 }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
	statechart_exit(&sc_handle);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
