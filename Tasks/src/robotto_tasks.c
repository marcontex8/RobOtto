/*
 * robotto_tasks.c
 *
 *  Created on: Nov 8, 2025
 *      Author: marco
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"

#include "i2c_busses.h"

#include "robotto_common.h"
#include "communication.h"

#include "SEGGER_RTT.h"

#include "main.h"

void runWheelsControlStateMachine();
#define WHEELS_CONTROL_PERIOD_MS 10
#define WHEELS_CONTROL_PRIORITY 10

void runPoseEstimationStateMachine();
#define POSE_ESTIMATION_PERIOD_MS 20
#define POSE_ESTIMATION_PRIORITY 8

void runObjectDetectionStateMachine();
#define OBJECT_DETECTION_PERIOD_MS 30
#define OBJECT_DETECTION_PRIORITY 6

void runMotionPlanningStateMachine();
#define MOTION_PLANNING_PERIOD_MS 50
#define MOTION_PLANNING_PRIORITY 4

void setCommunicationManagerQueue(QueueHandle_t communication_queue);
void runCommunicationManagerStateMachine();
#define COMMUNICATION_MANAGER_PRIORITY 2


#define LED_BLINK_PERIOD_MS 1000
#define LED_BLINK_PRIORITY 1

#define BUTTON_TASK_PRIORITY 1


TaskHandle_t led_task_handle = NULL;
TaskHandle_t motor_task_handle = NULL;
TaskHandle_t object_detection_handle = NULL;
TaskHandle_t motion_planning_handle = NULL;
TaskHandle_t buttonTaskHandle = NULL;
TaskHandle_t pose_estimation_handles = NULL;
TaskHandle_t communication_manager_handles = NULL;

// From Button to Motion Planning
QueueHandle_t behavior_queue_handle = NULL;
// From Motion Planning to Wheels Control
QueueHandle_t wheels_speed_set_points_queue_handle = NULL;
// From Wheels Control to Pose Estimation
QueueHandle_t wheels_status_queue_handle = NULL;
// From Pose Estimation to Motion Planning
QueueHandle_t robotto_pose_queue_handle = NULL;
// From Motion Planning to Telemetry
QueueHandle_t robotto_motion_telemetry_queue_handle = NULL;
// From Object Detection to Telemetry
QueueHandle_t robotto_object_detection_telemetry_queue_handle = NULL;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13)
    {
        static TickType_t last = 0;
        TickType_t now = xTaskGetTickCountFromISR();

        if ((now - last) > pdMS_TO_TICKS(50))
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;

            vTaskNotifyGiveFromISR(buttonTaskHandle, &xHigherPriorityTaskWoken);

            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        last = now;
    }
}


void ledBlinkTask(void *argument) {
	const TickType_t period = pdMS_TO_TICKS(LED_BLINK_PERIOD_MS);
	TickType_t last_wake_time = xTaskGetTickCount();
	for (;;) {
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		vTaskDelayUntil(&last_wake_time, period);
	}
}


void buttonTask(void *argument) {
    for (;;)
    {
    	static RobottoBehavior behavior = ROBOTTO_BEHAVIOR_IDLE;
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if(ROBOTTO_BEHAVIOR_IDLE == behavior)
        {
        	behavior = ROBOTTO_BEHAVIOR_RUNNING;
        }
        else // RUNNING
        {
        	behavior = ROBOTTO_BEHAVIOR_IDLE;
        }
        xQueueOverwrite(behavior_queue_handle, &behavior);
    }
}


void objectDetectionTask(void *argument)
{
	const TickType_t period = pdMS_TO_TICKS(OBJECT_DETECTION_PERIOD_MS);

	TickType_t last_wake_time = xTaskGetTickCount();
	for (;;) {
		runObjectDetectionStateMachine();
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


void wheelsControlTask(void *argument)
{
	const TickType_t period = pdMS_TO_TICKS(WHEELS_CONTROL_PERIOD_MS);

	TickType_t last_wake_time = xTaskGetTickCount();
	for (;;)
	{
		runWheelsControlStateMachine();
		vTaskDelayUntil(&last_wake_time, period);
	}
}

void poseEstimationTask(void *argument)
{
	const TickType_t period = pdMS_TO_TICKS(POSE_ESTIMATION_PERIOD_MS);

	TickType_t last_wake_time = xTaskGetTickCount();
	for (;;)
	{
		runPoseEstimationStateMachine();
		vTaskDelayUntil(&last_wake_time, period);
	}
}


RobottoErrorCode createSharedQueues()
{
	behavior_queue_handle = xQueueCreate(1, sizeof(RobottoBehavior));
	if (behavior_queue_handle == NULL)
	{
		return ROBOTTO_ERROR;
	}

	wheels_speed_set_points_queue_handle = xQueueCreate(1, sizeof(WheelSpeedSetPoint));
	if (wheels_speed_set_points_queue_handle == NULL)
	{
		return ROBOTTO_ERROR;
	}

	wheels_status_queue_handle = xQueueCreate(5, sizeof(WheelsMovementUpdate));
	if (wheels_status_queue_handle == NULL)
	{
		return ROBOTTO_ERROR;
	}

	robotto_pose_queue_handle = xQueueCreate(1, sizeof(RobottoPose));
	if (robotto_pose_queue_handle == NULL)
	{
		return ROBOTTO_ERROR;
	}

	robotto_motion_telemetry_queue_handle = xQueueCreate(1, sizeof(RobottoMotionTelemetry));
	if (robotto_motion_telemetry_queue_handle == NULL)
	{
		return ROBOTTO_ERROR;
	}

	robotto_object_detection_telemetry_queue_handle = xQueueCreate(1, sizeof(RobottoDetectionTelemetry));
	if (robotto_object_detection_telemetry_queue_handle == NULL)
	{
		return ROBOTTO_ERROR;
	}

	return ROBOTTO_OK;
}

RobottoErrorCode startTasks()
{
	if (xTaskCreate(ledBlinkTask, "LED_BLINK", configMINIMAL_STACK_SIZE,
			NULL, LED_BLINK_PRIORITY, &led_task_handle) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}
	if (xTaskCreate(buttonTask, "BUTTON", configMINIMAL_STACK_SIZE,
			NULL, BUTTON_TASK_PRIORITY, &buttonTaskHandle) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}
	if (xTaskCreate(wheelsControlTask, "WHEELS_CONTROL", configMINIMAL_STACK_SIZE,
			NULL, WHEELS_CONTROL_PRIORITY, &motor_task_handle) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}

	if (xTaskCreate(objectDetectionTask, "OBJECT_DETECTION", configMINIMAL_STACK_SIZE,
			NULL, OBJECT_DETECTION_PRIORITY, &object_detection_handle) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}
	if (xTaskCreate(motionPlanningTask, "MOTION_PLANNING", configMINIMAL_STACK_SIZE,
			NULL, MOTION_PLANNING_PRIORITY, &motion_planning_handle) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}
	if (xTaskCreate(poseEstimationTask, "POSE_ESTIMATION", configMINIMAL_STACK_SIZE,
			NULL, POSE_ESTIMATION_PRIORITY, &pose_estimation_handles) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}
	if (xTaskCreate(Communication_task, "COMMUNICATION_MANAGER", 4*configMINIMAL_STACK_SIZE,
			NULL, COMMUNICATION_MANAGER_PRIORITY, &communication_manager_handles) != pdPASS)
	{
		return ROBOTTO_ERROR;
	}
	return ROBOTTO_OK;
}

RobottoErrorCode initializeModules()
{
	initializeI2CMutexes();

	if(ROBOTTO_OK != createSharedQueues())
	{
		return ROBOTTO_ERROR;
	}

	if(ROBOTTO_OK != Communication_initializeCommunication())
	{
		return ROBOTTO_ERROR;
	}
	return ROBOTTO_OK;
}

RobottoErrorCode setupRobotto()
{
	if(ROBOTTO_OK != initializeModules())
	{
		return ROBOTTO_ERROR;
	}

	if(ROBOTTO_OK != startTasks())
	{
		return ROBOTTO_ERROR;
	}

	return ROBOTTO_OK;
}
