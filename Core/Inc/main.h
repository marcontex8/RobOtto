/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define MOTOR1_UP_Pin GPIO_PIN_0
#define MOTOR1_UP_GPIO_Port GPIOC
#define MOTOR1_DOWN_Pin GPIO_PIN_1
#define MOTOR1_DOWN_GPIO_Port GPIOC
#define MOTOR2_UP_Pin GPIO_PIN_2
#define MOTOR2_UP_GPIO_Port GPIOC
#define MOTOR2_DOWN_Pin GPIO_PIN_3
#define MOTOR2_DOWN_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define ENC2_SCL_Pin GPIO_PIN_10
#define ENC2_SCL_GPIO_Port GPIOB
#define MOTOR2_PWM_Pin GPIO_PIN_8
#define MOTOR2_PWM_GPIO_Port GPIOA
#define MOTOR1_PWM_Pin GPIO_PIN_9
#define MOTOR1_PWM_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define ENC2_SDA_Pin GPIO_PIN_12
#define ENC2_SDA_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define ENC1_SCL_Pin GPIO_PIN_6
#define ENC1_SCL_GPIO_Port GPIOB
#define ENC1_SDA_Pin GPIO_PIN_7
#define ENC1_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
