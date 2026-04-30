/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "robotto_tasks.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "robotto_conf.h"
#include "robotto_shared_queues.h"
#include "button_task.h"
#include "communication.h"

#include "main.h"

// Forward declaration of the tasks functions
void runMotionPlanningStateMachine();
void runObjectDetectionStateMachine();
void runWheelsControlStateMachine();
void runPoseEstimationStateMachine();

TaskHandle_t led_task_handle = NULL;
TaskHandle_t motor_task_handle = NULL;
TaskHandle_t object_detection_handle = NULL;
TaskHandle_t motion_planning_handle = NULL;
TaskHandle_t buttonTaskHandle = NULL;
TaskHandle_t pose_estimation_handles = NULL;
TaskHandle_t communication_manager_handles = NULL;

void ledBlinkTask(void *argument)
{
    const TickType_t period = pdMS_TO_TICKS(LED_BLINK_PERIOD_MS);
    TickType_t last_wake_time = xTaskGetTickCount();
    for (;;)
    {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        vTaskDelayUntil(&last_wake_time, period);
    }
}

void objectDetectionTask(void *argument)
{
    const TickType_t period = pdMS_TO_TICKS(OBJECT_DETECTION_PERIOD_MS);

    TickType_t last_wake_time = xTaskGetTickCount();
    for (;;)
    {
        runObjectDetectionStateMachine();
        vTaskDelayUntil(&last_wake_time, period);
    }
}

void motionPlanningTask(void *argument)
{
    const TickType_t period = pdMS_TO_TICKS(MOTION_PLANNING_PERIOD_MS);

    TickType_t last_wake_time = xTaskGetTickCount();
    for (;;)
    {
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

RobottoErrorCode startTasks()
{
    if (xTaskCreate(ledBlinkTask,
                    "LED_BLINK",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    LED_BLINK_PRIORITY,
                    &led_task_handle) != pdPASS)
    {
        return ROBOTTO_ERROR;
    }
    if (xTaskCreate(buttonTask,
                    "BUTTON",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    BUTTON_TASK_PRIORITY,
                    &buttonTaskHandle) != pdPASS)
    {
        return ROBOTTO_ERROR;
    }
    if (xTaskCreate(wheelsControlTask,
                    "WHEELS_CONTROL",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    WHEELS_CONTROL_PRIORITY,
                    &motor_task_handle) != pdPASS)
    {
        return ROBOTTO_ERROR;
    }

    if (xTaskCreate(objectDetectionTask,
                    "OBJECT_DETECTION",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    OBJECT_DETECTION_PRIORITY,
                    &object_detection_handle) != pdPASS)
    {
        return ROBOTTO_ERROR;
    }
    if (xTaskCreate(motionPlanningTask,
                    "MOTION_PLANNING",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    MOTION_PLANNING_PRIORITY,
                    &motion_planning_handle) != pdPASS)
    {
        return ROBOTTO_ERROR;
    }
    if (xTaskCreate(poseEstimationTask,
                    "POSE_ESTIMATION",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    POSE_ESTIMATION_PRIORITY,
                    &pose_estimation_handles) != pdPASS)
    {
        return ROBOTTO_ERROR;
    }
    if (xTaskCreate(Communication_task,
                    "COMMUNICATION_MANAGER",
                    4 * configMINIMAL_STACK_SIZE,
                    NULL,
                    COMMUNICATION_MANAGER_PRIORITY,
                    &communication_manager_handles) != pdPASS)
    {
        return ROBOTTO_ERROR;
    }
    return ROBOTTO_OK;
}
