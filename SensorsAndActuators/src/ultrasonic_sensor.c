/*
 * ultrasonic_sensor.c
 *
 *  Created on: Dec 28, 2025
 *      Author: marco
 */
#include "ultrasonic_sensor.h"

#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_hal.h"
#include <stdbool.h>

typedef enum
{
    ECHO_IDLE,
    ECHO_WAIT_RISING,
    ECHO_WAIT_FALLING
} EchoCaptureState;

extern TIM_HandleTypeDef htim10;





static volatile uint32_t echo_start_us = 0;
static volatile uint32_t echo_end_us   = 0;
static volatile bool measurement_done = false;


static volatile EchoCaptureState echo_state = ECHO_IDLE;

RobottoErrorCode getMeasurementIfReady(uint32_t* time)
{
	RobottoErrorCode result = ROBOTTO_ERROR;
	taskENTER_CRITICAL();
	if(measurement_done)
	{
		*time = echo_end_us - echo_start_us;
		measurement_done = false;
		result = ROBOTTO_OK;
	}
	taskEXIT_CRITICAL();
	return result;
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM10 ||
        htim->Channel != HAL_TIM_ACTIVE_CHANNEL_1)
    {
    	return;
    }

    uint32_t captured = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

    if (echo_state == ECHO_WAIT_RISING)
    {
        echo_start_us = captured;
        echo_state = ECHO_WAIT_FALLING;

        __HAL_TIM_SET_CAPTUREPOLARITY(
            htim,
            TIM_CHANNEL_1,
            TIM_INPUTCHANNELPOLARITY_FALLING
        );
    }
    else if(echo_state == ECHO_WAIT_FALLING)
    {
        echo_end_us = captured;
        echo_state = ECHO_IDLE;

        __HAL_TIM_SET_CAPTUREPOLARITY(
            htim,
            TIM_CHANNEL_1,
            TIM_INPUTCHANNELPOLARITY_RISING
        );
        measurement_done = true;
    }
    else
    {
    	// no action
    }
}


void triggerSensor(void)
{
	measurement_done = false;
    echo_state = ECHO_WAIT_RISING;

    __HAL_TIM_SET_CAPTUREPOLARITY(
        &htim10,
        TIM_CHANNEL_1,
        TIM_INPUTCHANNELPOLARITY_RISING
    );

    __HAL_TIM_CLEAR_FLAG(&htim10, TIM_FLAG_CC1);

    HAL_GPIO_WritePin(US_TRIGGER_GPIO_Port, US_TRIGGER_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(US_TRIGGER_GPIO_Port, US_TRIGGER_Pin, GPIO_PIN_RESET);
}

