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

#include "SEGGER_SYSVIEW.h"


extern TIM_HandleTypeDef htim10;
#define ECHO_TIMER_HANDLE htim10

extern TIM_HandleTypeDef htim11;
#define TRIGGER_TIMER_HANDLE htim11

#define CONVERSION_FACTOR 5831.0f


typedef enum
{
    ECHO_IDLE,
    ECHO_WAIT_RISING,
    ECHO_WAIT_FALLING
} EchoCaptureState;

static volatile uint32_t echo_start_us = 0;
static volatile uint32_t echo_end_us   = 0;
static volatile bool measurement_done = false;


static volatile EchoCaptureState echo_state = ECHO_IDLE;

RobottoErrorCode getMeasurementIfReady(float* distance)
{
	RobottoErrorCode result = ROBOTTO_ERROR;
	if(measurement_done)
	{
		*distance = (echo_end_us - echo_start_us) / CONVERSION_FACTOR;
		measurement_done = false;
		result = ROBOTTO_OK;
	}
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

void resetEchoTimer()
{
    __HAL_TIM_SET_COUNTER(&ECHO_TIMER_HANDLE, 0);

    __HAL_TIM_SET_CAPTUREPOLARITY(
        &ECHO_TIMER_HANDLE,
        TIM_CHANNEL_1,
        TIM_INPUTCHANNELPOLARITY_RISING
    );

    __HAL_TIM_CLEAR_FLAG(&ECHO_TIMER_HANDLE, TIM_FLAG_CC1);

    HAL_TIM_IC_Start_IT(&ECHO_TIMER_HANDLE, TIM_CHANNEL_1);

    __HAL_TIM_ENABLE(&ECHO_TIMER_HANDLE);
}

void startTriggerPulse()
{
	HAL_TIM_PWM_Stop(&TRIGGER_TIMER_HANDLE, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TRIGGER_TIMER_HANDLE, TIM_CHANNEL_1);
}

void triggerSensor()
{
	resetEchoTimer();
	measurement_done = false;
    echo_state = ECHO_WAIT_RISING;
    startTriggerPulse();
}
