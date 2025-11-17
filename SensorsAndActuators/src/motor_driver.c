/*
 * motor_driver.c
 *
 *  Created on: Nov 16, 2025
 *      Author: marco
 */


#include <math.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_gpio.h"
#include "motor_driver.h"


#define MOTOR_L_UP_Pin GPIO_PIN_0
#define MOTOR_L_UP_GPIO_Port GPIOC
#define MOTOR_L_DOWN_Pin GPIO_PIN_1
#define MOTOR_L_DOWN_GPIO_Port GPIOC
#define MOTOR_R_UP_Pin GPIO_PIN_2
#define MOTOR_R_UP_GPIO_Port GPIOC
#define MOTOR_R_DOWN_Pin GPIO_PIN_3
#define MOTOR_R_DOWN_GPIO_Port GPIOC
#define MOTOR_R_PWM_Pin GPIO_PIN_8
#define MOTOR_R_PWM_GPIO_Port GPIOA
#define MOTOR_L_PWM_Pin GPIO_PIN_9
#define MOTOR_L_PWM_GPIO_Port GPIOA


extern TIM_HandleTypeDef htim1;



#define MIN_PWM 300
#define MAX_PWM 499


void initializeMotorDriver()
{
	setMotorDutyCycle(WHEEL_RIGHT, 0);
	setMotorDutyCycle(WHEEL_LEFT, 0);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}


void setWheelDirection(Wheel wheel, WheelDirection direction)
{
	if(WHEEL_RIGHT == wheel)
	{
		if(WHEEL_FORWARD == direction)
		{
			HAL_GPIO_WritePin(MOTOR_R_UP_GPIO_Port, MOTOR_R_UP_Pin, 1);
			HAL_GPIO_WritePin(MOTOR_R_DOWN_GPIO_Port, MOTOR_R_DOWN_Pin, 0);
		}
		else
		{
			HAL_GPIO_WritePin(MOTOR_R_UP_GPIO_Port, MOTOR_R_UP_Pin, 0);
			HAL_GPIO_WritePin(MOTOR_R_DOWN_GPIO_Port, MOTOR_R_DOWN_Pin, 1);
		}
	}
	else if(WHEEL_LEFT == wheel)
	{
		if(WHEEL_FORWARD == direction)
		{
			HAL_GPIO_WritePin(MOTOR_L_UP_GPIO_Port, MOTOR_L_UP_Pin, 0);
			HAL_GPIO_WritePin(MOTOR_L_DOWN_GPIO_Port, MOTOR_L_DOWN_Pin, 1);
		}
		else
		{
			HAL_GPIO_WritePin(MOTOR_L_UP_GPIO_Port, MOTOR_L_UP_Pin, 1);
			HAL_GPIO_WritePin(MOTOR_L_DOWN_GPIO_Port, MOTOR_L_DOWN_Pin, 0);
		}
	}
}

void setMotorDutyCycle(Wheel wheel, float normalized_duty_cycle)
{
	if (normalized_duty_cycle > 1.0f) normalized_duty_cycle = 1.0f;
	if (normalized_duty_cycle < -1.0f) normalized_duty_cycle = -1.0f;

	WheelDirection direction = WHEEL_FORWARD;
	if(normalized_duty_cycle < 0.0f)
	{
		direction = WHEEL_BACKWARD;
	}
	setWheelDirection(wheel, direction);

	unsigned int motor_pwm = (unsigned int)((float)MAX_PWM * fabsf(normalized_duty_cycle));

	if(WHEEL_LEFT == wheel)
	{
		TIM1->CCR2 = motor_pwm;
	}
	else if(WHEEL_RIGHT == wheel)
	{
		TIM1->CCR1 = motor_pwm;
	}
}
