/*
 * servo.c
 *
 *  Created on: Dec 29, 2025
 *      Author: marco
 */
#include "stm32f4xx_hal.h"

#include "SEGGER_SYSVIEW.h"

extern TIM_HandleTypeDef htim13;
#define SERVO_TIMER_HANDLE htim13

#define SERVO_MIN_PERIOD 600
#define SERVO_MAX_PERIOD 2300




void initServo()
{
	HAL_TIM_PWM_Start(&SERVO_TIMER_HANDLE, TIM_CHANNEL_1);
}

void setServoAngle(float angle_deg)
{
	if(angle_deg > 90.0f || angle_deg < -90.0f)
	{
		return;
	}

	unsigned int period_range = SERVO_MAX_PERIOD - SERVO_MIN_PERIOD;

	unsigned int period = (unsigned int)((angle_deg + 90.0f) / 180.0f * (float)period_range) + SERVO_MIN_PERIOD;

	SERVO_TIMER_HANDLE.Instance->CCR1 = period;
}
