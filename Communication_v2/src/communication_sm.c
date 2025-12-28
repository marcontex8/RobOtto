/*
 * communication_sm.c
 *
 *  Created on: Dec 18, 2025
 *      Author: marco
 */

#include "robotto_conf.h"
#include "communication_events.h"

#include "FreeRTOS.h"
#include "timers.h"


typedef enum{
	COMM_STATE_OFF,
	COMM_STATE_STARTING_RX,
	COMM_STATE_CONNECTING,
	COMM_STATE_COMMAND_DELAY,
	COMM_STATE_COMMUNICATION_REAY,
	COMM_STATE_COUNT,
} CommunicationState;

static CommunicationState state = COMM_STATE_OFF;

static unsigned int current_command_id = 0;


static TimerHandle_t timer;


/**
 * Other useful commands
		AT+CWJAP=[<ssid>],[<pwd>][,<bssid>][,<pci_en>][,<reconn_interval>][,<listen_interval>][,<scan_mode>][,<jap_timeout>][,<pmf>]
		AT+CWJAP=<ssid>,<pwd>[,<bssid>][,<pci_en>][,<reconn>][,<listen_interval>][,<scan_mode>]
		"AT+CWLAP", // list access points
		"AT+RESTORE", // restore to factory settings
 */


static const char* init_commands[] = {
		"AT+RESTORE",
		"AT",
		"AT+CWMODE=1",
		"AT+CWQAP",
		"AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PWD "\"",
		"AT+MQTTUSERCFG=0,1,\"RobOTTO\",\"\",\"\",0,0,\"\"",
		"AT+MQTTCONN=0,\"" MQTT_BROKER_IP "\"," MQTT_BROKER_PORT ",0",
		"AT+MQTTPUB=0,\"test/topic\",\"Hello from RobOtto\",0,0"
};
static const size_t number_of_commands = sizeof(init_commands)/sizeof(init_commands[0]);


void timerExpired(TimerHandle_t timer_handle)
{
	postNewCommunicationEventWithNoData(EVENT_COMM_DELAY_EXPIRED);
}


CommunicationState onCommunicationInit(const CommunicationEventData*)
{
	postNewCommunicationEventWithNoData(EVENT_UART_RX_START_REQUEST);
	timer = xTimerCreate("Commands delay timer",
			pdMS_TO_TICKS(NETWORK_COMMANDS_DELAY_S * 1000),
			pdFALSE,
			NULL,
			&timerExpired);
	return COMM_STATE_STARTING_RX;
}

CommunicationState onUartRxStarted(const CommunicationEventData*)
{
	current_command_id = 0;
	ATRequestData request_data = {.buffer = init_commands[current_command_id], .request_id = current_command_id};
	CommunicationEventData data_to_send = {.at_request = request_data};
	postNewCommunicationEvent(EVENT_AT_NEW_REQUEST, data_to_send);
	return COMM_STATE_CONNECTING;
}

CommunicationState onConnectionStepATResponse(const CommunicationEventData* data)
{
	unsigned int request_id = data->at_response.request_id;
	ATResponseResult response = data->at_response.response;

	if(request_id != current_command_id)
	{
		// something is wrong. Start connection from the beginning
		current_command_id = 0;
	}
	else
	{
		if(AT_SUCCESS == response)
		{
			++current_command_id;
		}
		else
		{
			// re-issue the same current_command_id
		}
	}

	if(current_command_id < number_of_commands)
	{
		xTimerStart(timer, 0);
		return COMM_STATE_COMMAND_DELAY;
	}
	else
	{
		return COMM_STATE_COMMUNICATION_REAY;
	}
}


CommunicationState onDelayExpired(const CommunicationEventData*)
{
	ATRequestData request_data = {.buffer = init_commands[current_command_id], .request_id = current_command_id};
	CommunicationEventData data_to_send = {.at_request = request_data};
	SEGGER_SYSVIEW_PrintfHost("COMMAND: %s", data_to_send.at_request.buffer);
	postNewCommunicationEvent(EVENT_AT_NEW_REQUEST, data_to_send);
	return COMM_STATE_CONNECTING;
}

CommunicationState onNewATResponse(const CommunicationEventData* data)
{
	// TODO: implement normal communication
	return COMM_STATE_COMMUNICATION_REAY;
}

typedef CommunicationState (*CommStateTransitionFunctionPtr)(const CommunicationEventData* data);


static const CommStateTransitionFunctionPtr comm_state_transition_table[COMM_STATE_COUNT][EVENT_COUNT] = {
	    [COMM_STATE_OFF] = {
	        [EVENT_COMM_INIT] = onCommunicationInit,
	    },
		[COMM_STATE_STARTING_RX] = {
	        [EVENT_UART_RX_STARTED] = onUartRxStarted,
	    },
	    [COMM_STATE_CONNECTING] = {
	        [EVENT_AT_REQUEST_COMPLETE] = onConnectionStepATResponse,
	    },
		[COMM_STATE_COMMAND_DELAY] = {
			[EVENT_COMM_DELAY_EXPIRED] = onDelayExpired,
		},
		[COMM_STATE_COMMUNICATION_REAY] = {
		    [EVENT_AT_REQUEST_COMPLETE] = onNewATResponse,
		},
};

void communication_handleEvent(const CommunicationEvent* event)
{
	CommStateTransitionFunctionPtr function = comm_state_transition_table[state][event->id];
	if(function != NULL)
	{
		state = function(&(event->data));
	}
}
