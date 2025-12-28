/*
 * at_sm.c
 *
 *  Created on: Dec 18, 2025
 *      Author: marco
 */

#include "communication_events.h"

#include "buffer_parser.h"
#include "robotto_conf.h"
#include "robotto_common.h"

#include <string.h>
#include <math.h>

#include "FreeRTOS.h"
#include "timers.h"

#include "SEGGER_SYSVIEW.h"

typedef enum{
	AT_STATE_OFF,
	AT_STATE_IDLE,
	AT_STATE_WAITING_RESPONSE,
	AT_STATE_COUNT
} ATState;

static ATState state = AT_STATE_OFF;
static unsigned int latest_request = 0;

/* REPLY BUFFER VARIABLES */
#define REPLY_BUFFER_SIZE 1024
static uint8_t reply_buffer[REPLY_BUFFER_SIZE];
#define MAX_REPLY_BUFFER_SIZE (REPLY_BUFFER_SIZE -1) // at least 1 termination char should be always present in the buffer
static uint16_t reply_buffer_current_length = 0;

/*REQUEST BUFFER VARIABLES */
#define REQUEST_BUFFER_SIZE 256
static uint8_t request_buffer[REQUEST_BUFFER_SIZE];

// TIMER
static TimerHandle_t timer;


void timeoutExpired(TimerHandle_t timer_handle)
{
	// TODO add a check on the timer_handle?
	postNewCommunicationEventWithNoData(EVENT_AT_REQUEST_TIMEOUT);
}


void makeNewTxRequest(const CommunicationEventData* data)
{
	size_t size = strlen(data->at_request.buffer);
	latest_request = data->at_request.request_id;

	// crop message if it's too long to fit buffer and termination chars. Should never happen
	ROBOTTO_ASSERT(size <= REQUEST_BUFFER_SIZE-2);
	if(size > REQUEST_BUFFER_SIZE-2)
	{
		size = REQUEST_BUFFER_SIZE-2;
	}

	memcpy(request_buffer, data->at_request.buffer, size);
	request_buffer[size++] = '\r';
	request_buffer[size++] = '\n';
	memset(&request_buffer[size], 0, REQUEST_BUFFER_SIZE-size);

	memset(reply_buffer, 0, REPLY_BUFFER_SIZE);
	reply_buffer_current_length = 0;

	UartTxData uart_tx_data = {.buffer = request_buffer, .size = size};
	CommunicationEventData data_to_send = {.uart_tx = uart_tx_data};
	postNewCommunicationEvent(EVENT_UART_TX_REQUEST, data_to_send);

	if (timer == NULL || xTimerStart(timer, 0) != pdPASS)
	{
		SEGGER_SYSVIEW_Print("Something wrong with timer creation or starting");
	}
}


ATState onNewATRequest(const CommunicationEventData* data)
{
	makeNewTxRequest(data);
	return AT_STATE_WAITING_RESPONSE;
}


ATState onFirstATRequest(const CommunicationEventData* data)
{
	timer = xTimerCreate("AT request timer",
			pdMS_TO_TICKS(NETWORK_REQUEST_TIMEOUT_S * 1000),
			pdFALSE,
			NULL,
			&timeoutExpired);
	return onNewATRequest(data);
}

void copyChunkToLocalBuffer(const uint8_t* source_buffer, size_t size)
{
	if(size + reply_buffer_current_length > MAX_REPLY_BUFFER_SIZE)
	{
		size = MAX_REPLY_BUFFER_SIZE - reply_buffer_current_length;
	}
	memcpy(&reply_buffer[reply_buffer_current_length], source_buffer, size);
	reply_buffer_current_length += size;
}

void copyDataToLocalBuffer(const CommunicationEventData* data)
{
	const UartRxData* uart_data = &(data->uart_rx);
	if(uart_data->first_chunk_start != NULL)
	{
		copyChunkToLocalBuffer(uart_data->first_chunk_start, uart_data->first_chunk_size);
		if(uart_data->second_chunk_start != NULL)
		{
			copyChunkToLocalBuffer(uart_data->second_chunk_start, uart_data->second_chunk_size);
		}
	}
	PRINT_LINES_ON_SYSTEMVIEW((const char *)reply_buffer);
}

ATState onNewExpectedDataReceived(const CommunicationEventData* data)
{
	ATState next_state = AT_STATE_WAITING_RESPONSE;

	copyDataToLocalBuffer(data);
	if(NULL != findLineInBuffer((const char *)reply_buffer, "OK"))
	{
	  	SEGGER_SYSVIEW_Print("found OK");
		xTimerStop(timer, 0);

		ATResponseData at_response_data = {.request_id = latest_request, .response = AT_SUCCESS};
		CommunicationEventData data_to_send = {.at_response = at_response_data};
		postNewCommunicationEvent(EVENT_AT_REQUEST_COMPLETE, data_to_send);
		next_state = AT_STATE_IDLE;
	}
	else if(NULL != findLineInBuffer((const char *)reply_buffer, "ERROR"))
	{
	  	SEGGER_SYSVIEW_Print("found ERROR");
		xTimerStop(timer, 0);

		ATResponseData at_response_data = {.request_id = latest_request, .response = AT_FAILURE};
		CommunicationEventData data_to_send = {.at_response = at_response_data};
		postNewCommunicationEvent(EVENT_AT_REQUEST_COMPLETE, data_to_send);
		next_state = AT_STATE_IDLE;
	}
	else
	{
	  	SEGGER_SYSVIEW_Print("found NOTHING");
	}
	return next_state;
}

ATState onAnyError(const CommunicationEventData* data)
{
	ATResponseData at_response_data = {.response = AT_FAILURE};
	CommunicationEventData data_to_send = {.at_response = at_response_data};

	postNewCommunicationEvent(EVENT_AT_REQUEST_COMPLETE, data_to_send);
	return AT_STATE_IDLE;
}

typedef ATState (*ATStateTransitionFunction)(const CommunicationEventData* data);

static const ATStateTransitionFunction at_state_transition_table[AT_STATE_COUNT][EVENT_COUNT] = {
    [AT_STATE_OFF] = {
        [EVENT_AT_NEW_REQUEST] = onFirstATRequest,
    },
    [AT_STATE_IDLE] = {
        [EVENT_AT_NEW_REQUEST] = onNewATRequest,
		[EVENT_UART_RX_NEW_DATA_RECEIVED] = NULL, // TODO: manage unexpected communications
    },
    [AT_STATE_WAITING_RESPONSE] = {
    	[EVENT_AT_NEW_REQUEST] = NULL, // TODO: this should be monitored as an error!
		[EVENT_UART_TX_COMPLETE] = NULL, // at the moment we assume this will be fine
		[EVENT_UART_TX_ERROR] = NULL, // at the moment we assume this will be fine
        [EVENT_UART_RX_ERROR]    = onAnyError,
        [EVENT_AT_REQUEST_TIMEOUT]    = onAnyError,
		[EVENT_UART_RX_NEW_DATA_RECEIVED] = onNewExpectedDataReceived,
    },
};



void at_handleEvent(const CommunicationEvent* event)
{
	ATStateTransitionFunction function = at_state_transition_table[state][event->id];
	if(function != NULL)
	{
		state = function(&(event->data));
	}
}

