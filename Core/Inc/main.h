/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

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
#define Ampl_IN_Pin GPIO_PIN_0
#define Ampl_IN_GPIO_Port GPIOA
#define RMS_IN_Pin GPIO_PIN_1
#define RMS_IN_GPIO_Port GPIOA
#define Data_300mV_Pin GPIO_PIN_6
#define Data_300mV_GPIO_Port GPIOA
#define Data_1V_Pin GPIO_PIN_7
#define Data_1V_GPIO_Port GPIOA
#define Data_30mV_Pin GPIO_PIN_10
#define Data_30mV_GPIO_Port GPIOB
#define Data_10mV_Pin GPIO_PIN_7
#define Data_10mV_GPIO_Port GPIOC
#define Data_10V_Pin GPIO_PIN_8
#define Data_10V_GPIO_Port GPIOA
#define Data_3V_Pin GPIO_PIN_9
#define Data_3V_GPIO_Port GPIOA
#define Data_100mV_Pin GPIO_PIN_4
#define Data_100mV_GPIO_Port GPIOB
#define EEPROM_WP_Pin GPIO_PIN_5
#define EEPROM_WP_GPIO_Port GPIOB
#define Data_3mV_Pin GPIO_PIN_6
#define Data_3mV_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
