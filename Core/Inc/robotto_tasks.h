/*
 * robotto_tasks.h
 *
 *  Created on: Nov 8, 2025
 *      Author: marco
 */

#ifndef INC_ROBOTTO_TASKS_H_
#define INC_ROBOTTO_TASKS_H_


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "robotto_common.h"

extern void runMotionPlanningStateMachine();
#define LED_BLINK_PERIOD_MS 1000
#define LED_BLINK_PRIORITY 1

extern void runMotorControlStateMachine();
#define MOTOR_CONTROL_PERIOD_MS 10
#define MOTOR_CONTROL_PRIORITY 5

#define MOTION_PLANNING_PERIOD_MS 50
#define MOTION_PLANNING_PRIORITY 3



TaskHandle_t led_task_handle = NULL;
TaskHandle_t motor_task_handle = NULL;
TaskHandle_t motion_planning_handle = NULL;

QueueHandle_t speed_set_points_queue_handle = NULL;


void startLedBlinkTask(void *argument) {
	const TickType_t period = pdMS_TO_TICKS(LED_BLINK_PERIOD_MS);
	TickType_t last_wake_time = xTaskGetTickCount();
	for (;;) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		vTaskDelayUntil(&last_wake_time, period);
	}
}

void motorControlTask(void *argument)
{
	const TickType_t period = pdMS_TO_TICKS(MOTOR_CONTROL_PERIOD_MS);

	TickType_t last_wake_time = xTaskGetTickCount();
	for (;;)
	{
		runMotorControlStateMachine();
		vTaskDelayUntil(&last_wake_time, period);
	}
}

void motionPlanningTask(void *argument)
{
	const TickType_t period = pdMS_TO_TICKS(MOTION_PLANNING_PERIOD_MS);

	TickType_t last_wake_time = xTaskGetTickCount();
	for (;;) {
		runMotionPlanningStateMachine();
		vTaskDelayUntil(&last_wake_time, period);
	}
}





RobottoErrorCode createRobottoTasks()
{
	speed_set_points_queue_handle = xQueueCreate(1, sizeof(WheelSpeedSetPoint));
	if (speed_set_points_queue_handle == NULL)
	{
		return ROBOTTO_ERROR;
	}


	if (xTaskCreate(startLedBlinkTask, "LED_Blink", configMINIMAL_STACK_SIZE,
			NULL, LED_BLINK_PRIORITY, &led_task_handle) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}
	if (xTaskCreate(motorControlTask, "MOTOR_Control", configMINIMAL_STACK_SIZE,
			NULL, MOTOR_CONTROL_PRIORITY, &motor_task_handle) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}
	if (xTaskCreate(motionPlanningTask, "MOTION_Planning", configMINIMAL_STACK_SIZE,
			NULL, MOTION_PLANNING_PRIORITY, &motion_planning_handle) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}
	return ROBOTTO_OK;
}

#endif /* INC_ROBOTTO_TASKS_H_ */
