/*
 * telemetry_sm.c
 *
 *  Created on: Jan 12, 2026
 *      Author: marco
 */

#include "robotto_conf.h"
#include "robotto_common.h"
#include "communication_events.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"

#include <stdio.h>
#include <inttypes.h>

typedef enum{
	TELEMETRY_STATE_OFF,
	TELEMETRY_STATE_ON,

	TELEMETRY_STATE_COUNT,
	} TelemetryState;

static TelemetryState state = TELEMETRY_STATE_OFF;

static TimerHandle_t telemetry_timer;

extern QueueHandle_t robotto_telemetry_pose_queue_handle;

static char json[200];
static char at_command[256];

void telemetryTimerCallback()
{
	postNewCommunicationEventWithNoData(EVENT_TELEMETRY_TICK);
}

TelemetryState onConnectionEstablished(const CommunicationEventData* data)
{
	telemetry_timer = xTimerCreate(
	    "telemetry_timer",
	    pdMS_TO_TICKS(TELEMETRY_PERIOD_MS),
	    pdTRUE,
	    NULL,
		telemetryTimerCallback
	);
	xTimerStart(telemetry_timer, 0);
	return TELEMETRY_STATE_ON;
}

TelemetryState onTelemetryTick(const CommunicationEventData* data)
{
	RobottoPose pose;
	if (pdTRUE == xQueuePeek(robotto_telemetry_pose_queue_handle, &pose, 0))
	{
		snprintf(json, sizeof(json),
		         "{\"timestamp\": %" PRIu32 ", \"x\": %.2f, \"y\": %.2f, \"theta\": %.2f}",
		         pose.timestamp, pose.x, pose.y, pose.theta);

		snprintf(at_command, sizeof(at_command),
		         "AT+MQTTPUB=0,\"%s\",\"\\\"%s\\\"\",0,0\r\n",
		         "RobOtto/pose",
		         json);

	    ATRequestData request_data = {.buffer = at_command, .request_id=101};
		CommunicationEventData data_to_send = {.at_request = request_data};
		postNewCommunicationEvent(EVENT_AT_NEW_REQUEST, data_to_send);
	}


	return TELEMETRY_STATE_ON;
}


typedef TelemetryState (*TelemetryStateTransitionFunctionPtr)(const CommunicationEventData* data);

static const TelemetryStateTransitionFunctionPtr telemetry_state_transition_table[TELEMETRY_STATE_COUNT][EVENT_COUNT] = {
	    [TELEMETRY_STATE_OFF] = {
	        [EVENT_CONNECTION_ESTABLISHED] = onConnectionEstablished,
	    },
		[TELEMETRY_STATE_ON] = {
	        [EVENT_TELEMETRY_TICK] = onTelemetryTick,
	    },
};

void telemetry_handleEvent(const CommunicationEvent* event)
{
	TelemetryStateTransitionFunctionPtr function = telemetry_state_transition_table[state][event->id];
	if(function != NULL)
	{
		state = function(&(event->data));
	}
}
