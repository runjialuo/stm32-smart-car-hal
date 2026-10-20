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
#include "stm32f1xx_hal.h"

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
#define tb6612fng_AIN2_Pin GPIO_PIN_4
#define tb6612fng_AIN2_GPIO_Port GPIOA
#define tb6612fng_AIN1_Pin GPIO_PIN_5
#define tb6612fng_AIN1_GPIO_Port GPIOA
#define tb6612fng_BIN1_Pin GPIO_PIN_6
#define tb6612fng_BIN1_GPIO_Port GPIOA
#define tb6612fng_BIN2_Pin GPIO_PIN_7
#define tb6612fng_BIN2_GPIO_Port GPIOA
#define IR_X4_Pin GPIO_PIN_0
#define IR_X4_GPIO_Port GPIOB
#define IR_X3_Pin GPIO_PIN_1
#define IR_X3_GPIO_Port GPIOB
#define IR_X2_Pin GPIO_PIN_10
#define IR_X2_GPIO_Port GPIOB
#define IR_X1_Pin GPIO_PIN_11
#define IR_X1_GPIO_Port GPIOB
#define HC_SR04_Trig_Pin GPIO_PIN_12
#define HC_SR04_Trig_GPIO_Port GPIOB
#define HC_SR04_Echo_Pin GPIO_PIN_13
#define HC_SR04_Echo_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_8
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_9
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
