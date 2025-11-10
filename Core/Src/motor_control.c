/*
 * motor_control.c
 *
 *  Created on: Nov 6, 2025
 *      Author: marco
 */

#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "encoder_reader.h"
#include "robotto_common.h"
#include "SEGGER_RTT.h"
#include <stdbool.h>
#include "odometry.h"

#include "queue.h"


#define MIN_PWM 300
#define MAX_PWM 499

typedef enum {
	WHEEL_RIGHT,
	WHEEL_LEFT,
} Wheel;

typedef enum{
	WHEEL_FORWARD,
	WHEEL_BACKWARD,
} WheelDirection;

extern TIM_HandleTypeDef htim1;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern QueueHandle_t speed_set_points_queue_handle;

static WheelStatus left_wheel = {0};
static WheelStatus right_wheel = {0};
static char* last_error = NULL;


void setWheelDirection(Wheel wheel, WheelDirection direction)
{
	if(WHEEL_RIGHT == wheel)
	{
		if(WHEEL_FORWARD == direction)
		{
			HAL_GPIO_WritePin(MOTOR2_UP_GPIO_Port, MOTOR2_UP_Pin, 1);
			HAL_GPIO_WritePin(MOTOR2_DOWN_GPIO_Port, MOTOR2_DOWN_Pin, 0);
		}
		else
		{
			HAL_GPIO_WritePin(MOTOR2_UP_GPIO_Port, MOTOR2_UP_Pin, 0);
			HAL_GPIO_WritePin(MOTOR2_DOWN_GPIO_Port, MOTOR2_DOWN_Pin, 1);
		}
	}
	else if(WHEEL_LEFT == wheel)
	{
		if(WHEEL_FORWARD == direction)
		{
			HAL_GPIO_WritePin(MOTOR1_UP_GPIO_Port, MOTOR1_UP_Pin, 1);
			HAL_GPIO_WritePin(MOTOR1_DOWN_GPIO_Port, MOTOR1_DOWN_Pin, 0);
		}
		else
		{
			HAL_GPIO_WritePin(MOTOR1_UP_GPIO_Port, MOTOR1_UP_Pin, 0);
			HAL_GPIO_WritePin(MOTOR1_DOWN_GPIO_Port, MOTOR1_DOWN_Pin, 1);
		}
	}
}


void setWheelNormalizedPWM(Wheel wheel, int speed)
{
	if (speed > 100) speed = 100;
	if (speed < -100) speed = -100;

	WheelDirection direction = WHEEL_FORWARD;
	if(speed < 0)
	{
		direction = WHEEL_BACKWARD;
	}
	setWheelDirection(wheel, direction);

	int motor_pwm = MAX_PWM * abs(speed) / 100;
	if(WHEEL_LEFT == wheel)
	{
		TIM1->CCR1 = motor_pwm;
	}
	else if(WHEEL_RIGHT == wheel)
	{
		TIM1->CCR2 = motor_pwm;
	}
}


ActivityStatus motorControlStatusInit()
{
	setWheelNormalizedPWM(WHEEL_RIGHT, 0);
	setWheelNormalizedPWM(WHEEL_LEFT, 0);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);


	EncoderStatus right_encoder_status = {0};
	if(ROBOTTO_OK != readFullEncoder(&hi2c2, &right_encoder_status) || 0 == right_encoder_status.magnet_detected)
	{
		last_error = "ERROR WHILE READING RIGHT ENCODER";
		return ACTIVITY_STATUS_ERROR;
	};
	updateWheelStatus(&left_wheel, right_encoder_status.angle, xTaskGetTickCount());

	EncoderStatus left_encoder_status = {0};
	if(ROBOTTO_OK != readFullEncoder(&hi2c1, &left_encoder_status) || 0 == left_encoder_status.magnet_detected)
	{
		last_error = "ERROR WHILE READING LEFT ENCODER";
		return ACTIVITY_STATUS_ERROR;
	};
	updateWheelStatus(&right_wheel, left_encoder_status.angle, xTaskGetTickCount());

	return ACTIVITY_STATUS_RUNNING;
}


RobottoErrorCode updateWheelsStatus()
{
	TickType_t tick = xTaskGetTickCount();

	float left_wheel_angle = 0.0f;
	if(ROBOTTO_OK != readAngle(&hi2c1, &left_wheel_angle))
	{
		return ROBOTTO_ERROR;
	}
	updateWheelStatus(&left_wheel, left_wheel_angle, tick);


	float right_wheel_angle = 0.0f;
	if(ROBOTTO_OK != readAngle(&hi2c1, &right_wheel_angle))
	{
		return ROBOTTO_ERROR;
	}
	updateWheelStatus(&right_wheel, right_wheel_angle, tick);

	return ROBOTTO_OK;
}

ActivityStatus motorControlStatusRunning()
{
	static WheelSpeedSetPoint speed_set_point = {0};
	static int missing_setpoint_counter = 0;

	if (xQueueReceive(speed_set_points_queue_handle, &speed_set_point, 0) == pdPASS)
	{
		missing_setpoint_counter = 0;
	}
	else
	{
		++missing_setpoint_counter;
	}

	if(missing_setpoint_counter > 6)
	{
		last_error = "MISSING UPDATES OF SPEED SETPOINTS";
		return ACTIVITY_STATUS_ERROR;
	}

	if(ROBOTTO_OK != updateWheelsStatus())
	{
		last_error = "CANNOT READ FROM ENCODER";
		return ACTIVITY_STATUS_ERROR;
	}

	setWheelNormalizedPWM(WHEEL_RIGHT, (int)(speed_set_point.right*100));
	setWheelNormalizedPWM(WHEEL_LEFT, (int)(speed_set_point.left*100));

	//SEGGER_RTT_printf(0, "%u;%u;%d\n", tick, (int)(motor2_angle*1000), (int)(speed*1000));

	return ACTIVITY_STATUS_RUNNING;
}




void runMotorControlStateMachine()
{
	static ActivityStatus activity_status = ACTIVITY_STATUS_INIT;

	if(ACTIVITY_STATUS_INIT == activity_status)
	{
		activity_status = motorControlStatusInit();
		return;
	}
	else if(ACTIVITY_STATUS_RUNNING == activity_status)
	{
		activity_status = motorControlStatusRunning();
		return;
	}
	else
	{
		// ACTIVITY_STATUS_ERROR
	}
}
