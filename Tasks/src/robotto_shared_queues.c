/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "robotto_shared_queues.h"
#include "FreeRTOS.h"
#include "queue.h"

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

    robotto_object_detection_telemetry_queue_handle =
        xQueueCreate(1, sizeof(RobottoDetectionTelemetry));
    if (robotto_object_detection_telemetry_queue_handle == NULL)
    {
        return ROBOTTO_ERROR;
    }

    return ROBOTTO_OK;
}
