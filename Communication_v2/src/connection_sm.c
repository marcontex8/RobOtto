/*
 * connection_sm.c
 *
 *  Created on: Dec 18, 2025
 *      Author: marco
 */

#include "robotto_conf.h"
#include "communication_events.h"
#include "communication_queue.h"

#include "FreeRTOS.h"
#include "timers.h"


typedef enum{
	CONNECTION_STATE_OFF,
	CONNECTION_STATE_STARTING_RX,
	CONNECTION_STATE_CONNECTING,
	CONNECTION_STATE_COMMAND_DELAY,
	CONNECTION_STATE_CONNECTED,
	CONNECTION_STATE_COUNT,
} ConnectionState;

static ConnectionState state = CONNECTION_STATE_OFF;

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
		"ATE0",
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
	postNewCommunicationEventWithNoData(EVENT_CONNECTION_DELAY_EXPIRED);
}


ConnectionState onCommunicationInit(const CommunicationEventData*)
{
	postNewCommunicationEventWithNoData(EVENT_UART_RX_START_REQUEST);
	timer = xTimerCreate("Commands delay timer",
			pdMS_TO_TICKS(NETWORK_COMMANDS_DELAY_S * 1000),
			pdFALSE,
			NULL,
			&timerExpired);
	return CONNECTION_STATE_STARTING_RX;
}

ConnectionState onUartRxStarted(const CommunicationEventData*)
{
	current_command_id = 0;
	ATRequestData request_data = {.buffer = init_commands[current_command_id], .request_id = current_command_id};
	CommunicationEventData data_to_send = {.at_request = request_data};
	postNewCommunicationEvent(EVENT_AT_NEW_REQUEST, data_to_send);
	return CONNECTION_STATE_CONNECTING;
}

ConnectionState onConnectionStepATResponse(const CommunicationEventData* data)
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
		return CONNECTION_STATE_COMMAND_DELAY;
	}
	else
	{
		postNewCommunicationEventWithNoData(EVENT_CONNECTION_ESTABLISHED);
		return CONNECTION_STATE_CONNECTED;
	}
}


ConnectionState onDelayExpired(const CommunicationEventData*)
{
	ATRequestData request_data = {.buffer = init_commands[current_command_id], .request_id = current_command_id};
	CommunicationEventData data_to_send = {.at_request = request_data};
	SEGGER_SYSVIEW_PrintfHost("COMMAND: %s", data_to_send.at_request.buffer);
	postNewCommunicationEvent(EVENT_AT_NEW_REQUEST, data_to_send);
	return CONNECTION_STATE_CONNECTING;
}


typedef ConnectionState (*ConnectionStateTransitionFunctionPtr)(const CommunicationEventData* data);


static const ConnectionStateTransitionFunctionPtr comm_state_transition_table[CONNECTION_STATE_COUNT][EVENT_COUNT] = {
	    [CONNECTION_STATE_OFF] = {
	        [EVENT_CONNECTION_INIT] = onCommunicationInit,
	    },
		[CONNECTION_STATE_STARTING_RX] = {
	        [EVENT_UART_RX_STARTED] = onUartRxStarted,
	    },
	    [CONNECTION_STATE_CONNECTING] = {
	        [EVENT_AT_REQUEST_COMPLETE] = onConnectionStepATResponse,
	    },
		[CONNECTION_STATE_COMMAND_DELAY] = {
			[EVENT_CONNECTION_DELAY_EXPIRED] = onDelayExpired,
		},
		[CONNECTION_STATE_CONNECTED] = {}, // shall manage disconnection
};

void connection_handleEvent(const CommunicationEvent* event)
{
	ConnectionStateTransitionFunctionPtr function = comm_state_transition_table[state][event->id];
	if(function != NULL)
	{
		state = function(&(event->data));
	}
}
