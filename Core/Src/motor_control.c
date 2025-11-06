/*
 * motor_control.c
 *
 *  Created on: Nov 6, 2025
 *      Author: marco
 */
#include "motor_control.h"


#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "encoder_reader.h"
#include "common.h"
#include "SEGGER_RTT.h"
#include <stdbool.h>


extern TIM_HandleTypeDef htim1;

void MotorControlTask(void *argument)
{
	activity_status_t activity_status = ACTIVITY_STATUS_INIT;
	const TickType_t period = pdMS_TO_TICKS(10);


	TickType_t lastWakeTime = xTaskGetTickCount();
	for (;;)
	{
		if(activity_status == ACTIVITY_STATUS_INIT)
		{
			TIM1->CCR1 = 0;
			TIM1->CCR2 = 0;

			bool direction = true;
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
			HAL_GPIO_WritePin(MOTOR1_UP_GPIO_Port, MOTOR1_UP_Pin, direction);
			HAL_GPIO_WritePin(MOTOR1_DOWN_GPIO_Port, MOTOR1_DOWN_Pin, !direction);
			HAL_GPIO_WritePin(MOTOR2_UP_GPIO_Port, MOTOR2_UP_Pin, direction);
			HAL_GPIO_WritePin(MOTOR2_DOWN_GPIO_Port, MOTOR2_DOWN_Pin, !direction);

			encoder_reading_t encoder_status = {0};
			if(!read_full_encoder(&hi2c2, &encoder_status) || 0 == encoder_status.magnet_detected)
			{
				activity_status = ACTIVITY_STATUS_ERROR;
				continue;
			};
			activity_status = ACTIVITY_STATUS_RUNNING;
		}
		else if(activity_status == ACTIVITY_STATUS_RUNNING)
		{
			static uint32_t counter = 0;
			static float motor2_angle = 0.0f;

			if(!read_angle(&hi2c2, &motor2_angle))
			{
				activity_status = ACTIVITY_STATUS_ERROR;
				continue;
			}

			SEGGER_RTT_printf(0, "Motor 2: %d\n", (int)motor2_angle);


			if(counter % 100 < 50U) {
				TIM1->CCR1 = 0;
				TIM1->CCR2 = 0;
			} else {
				TIM1->CCR1 = 450;
				TIM1->CCR2 = 450;
			}
			counter++;
		}
		else
		{
			// ACTIVITY_STATUS_ERROR

		}

		vTaskDelayUntil(&lastWakeTime, period);
	}
}
