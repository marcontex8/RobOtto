/*
 * detection_manager.c
 *
 *  Created on: Dec 30, 2025
 *      Author: marco
 */
#include "robotto_common.h"

#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t robotto_object_detection_telemetry_queue_handle;

void addDetection(TickType_t time, float distance, float servo_angle)
{
	RobottoDetectionTelemetry detection = {
		.timestamp = time,
		.distance_m = distance,
		.servo_angle = servo_angle,
	};

	xQueueOverwrite(robotto_object_detection_telemetry_queue_handle, &detection);
}
