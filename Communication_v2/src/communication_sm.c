/*
 * communication_sm.c
 *
 *  Created on: Dec 18, 2025
 *      Author: marco
 */

#include "robotto_conf.h"
#include "communication_events.h"


typedef enum{
	COMM_STATE_OFF,
	COMM_STATE_STARTING_RX,
	COMM_STATE_CONNECTING,
	COMM_STATE_COMMUNICATION_REAY,
	COMM_STATE_COUNT
} CommunicationState;

static CommunicationState state = COMM_STATE_OFF;

/**
 * Other useful commands
		"AT+CWLAP", // list access points
		"AT+RESTORE", // restore to factory settings
 */
static const ATRequestData init_commands[] = {
		{.request_id = 1, .buffer = "AT"},
		{.request_id = 3, .buffer = "AT+CWMODE=1"},
		{.request_id = 4, .buffer = "AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PWD "\""},
		{.request_id = 5, .buffer = "AT+MQTTUSERCFG=0,1,\"RobOTTO\",\"\",\"\",0,0,\"\""},
		{.request_id = 6, .buffer = "AT+MQTTCONN=0,\"192.168.1.140\",1884,0"},
		{.request_id = 7, .buffer = "AT+MQTTPUB=0,\"test/topic\",\"Hello from RobOtto\",0,0"}
};
static const size_t number_of_commands = sizeof(init_commands)/sizeof(init_commands[0]);

CommunicationState onCommunicationInit(const void * data)
{
	postNewCommunicationEvent(EVENT_UART_RX_START_REQUEST, NULL);
	return COMM_STATE_STARTING_RX;
}

CommunicationState onUartRxStarted(const void * data)
{
	postNewCommunicationEvent(EVENT_AT_NEW_REQUEST, &init_commands[0]);
	return COMM_STATE_CONNECTING;
}

CommunicationState onConnectionStepATResponse(const void * data)
{
	static unsigned int command = 0;
	unsigned int request_id = command; // TODO once populated: ((ATResponseData*)(data))->request_id;
	ATResponseResult response = ((ATResponseData*)(data))->response;

	if(request_id != command)
	{
		// something is wrong. Start connection from the beginning
		command = 0;
	}
	else
	{
		if(AT_SUCCESS == response)
		{
			++command;
		}
		else
		{
			// re-issue the same command
		}
	}

	if(command < number_of_commands)
	{
		postNewCommunicationEvent(EVENT_AT_NEW_REQUEST, &init_commands[command]);
		return COMM_STATE_CONNECTING;
	}
	else
	{
		return COMM_STATE_COMMUNICATION_REAY;
	}
}


CommunicationState onNewATResponse(const void * data)
{
	// TODO: implement normal communication
	return COMM_STATE_COMMUNICATION_REAY;
}

typedef CommunicationState (*CommStateTransitionFunctionPtr)(const void* data);


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
		[COMM_STATE_COMMUNICATION_REAY] = {
		    [EVENT_AT_REQUEST_COMPLETE] = onNewATResponse,
		},
};

void communication_handleEvent(CommunicationEvent event)
{
	CommStateTransitionFunctionPtr function = comm_state_transition_table[state][event.id];
	if(function != NULL)
	{
		state = function(event.data);
	}
}
