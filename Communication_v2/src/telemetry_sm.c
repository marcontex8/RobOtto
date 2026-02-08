/*
 * telemetry_sm.c
 *
 *  Created on: Jan 12, 2026
 *      Author: marco
 */

#include "robotto_conf.h"
#include "robotto_common.h"
#include "communication_events.h"
#include "at_command_writer.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"


typedef enum{
	TELEMETRY_STATE_OFF,
	TELEMETRY_STATE_ON,

	TELEMETRY_STATE_COUNT,
	} TelemetryState;

static TelemetryState state = TELEMETRY_STATE_OFF;

static TimerHandle_t telemetry_timer;

extern QueueHandle_t robotto_telemetry_queue_handle;

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
	RobottoAggregatedTelemetry aggregated_telemetry;
	if (pdTRUE == xQueuePeek(robotto_telemetry_queue_handle, &aggregated_telemetry, 0))
	{
		atMqttPubFromTelemetry(&aggregated_telemetry.pose, &aggregated_telemetry.target_pose, &aggregated_telemetry.speed_set_point, MQTT_TOPIC_TELEMETRY, at_command, sizeof(at_command));
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
