/*
 * at_sm.c
 *
 *  Created on: Dec 18, 2025
 *      Author: marco
 */

#include "communication_events.h"

#include "buffer_parser.h"
#include "robotto_conf.h"

#include <string.h>

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

static UartTxData latest_tx_data = {0};
static ATResponseData latest_response = {0};

/* REPLY BUFFER VARIABLES */
#define BUFFER_SIZE 1024
#define MAX_WRITABLE_BUFFER_SIZE (BUFFER_SIZE -1) // at least 1 termination char should be always present in the buffer
static uint8_t reply_buffer[BUFFER_SIZE];
static uint16_t reply_buffer_current_length = 0;

void resetATBuffer()
{
	memset(reply_buffer, 0, BUFFER_SIZE);
	reply_buffer_current_length = 0;
}


// TIMER

static TimerHandle_t timer;
void timeoutExpired(TimerHandle_t timer_handle)
{
	// TODO add a check on the timer_handle?
	postNewCommunicationEvent(EVENT_AT_REQUEST_TIMEOUT, NULL);
}


void makeNewTxRequest(const void* data)
{
	resetATBuffer();

	ATRequestData* at_data = (ATRequestData*) data;
	latest_tx_data.size = strlen(at_data->buffer);
	latest_tx_data.buffer = (uint8_t*)at_data->buffer;

	postNewCommunicationEvent(EVENT_UART_TX_REQUEST, &latest_tx_data);

	if (timer == NULL || xTimerStart(timer, 0) != pdPASS)
	{
		SEGGER_SYSVIEW_Print("Something wrong with timer creation or starting");
	}
}


ATState onNewATRequest(const void* data)
{
	makeNewTxRequest(data);
	return AT_STATE_WAITING_RESPONSE;
}


ATState onFirstATRequest(const void* data)
{
	timer = xTimerCreate("AT request timer",
			pdMS_TO_TICKS(NETWORK_REQUEST_TIMEOUT_S * 1000),
			pdFALSE,
			NULL,
			&timeoutExpired);
	return onNewATRequest(data);
}

ATState onNewExpectedDataReceived(const void* data)
{
	ATState next_state = AT_STATE_WAITING_RESPONSE;
	UartRxData* uart_data = (UartRxData*) data;
	// TODO improve max size error check
	if(uart_data->first_chunk_start != NULL && uart_data->first_chunk_size + reply_buffer_current_length < MAX_WRITABLE_BUFFER_SIZE)
	{
		memcpy(&reply_buffer[reply_buffer_current_length], uart_data->first_chunk_start, uart_data->first_chunk_size);
	}
	reply_buffer_current_length += uart_data->first_chunk_size;
	if(uart_data->second_chunk_start != NULL && uart_data->second_chunk_size + reply_buffer_current_length < MAX_WRITABLE_BUFFER_SIZE)
	{
		memcpy(&reply_buffer[reply_buffer_current_length], uart_data->second_chunk_start, uart_data->second_chunk_size);
	}
	reply_buffer_current_length += uart_data->first_chunk_size;

  	SEGGER_SYSVIEW_PrintfHost("reply_buffer: %s", reply_buffer);

	if(NULL != findLineInBuffer((const char *)reply_buffer, "OK"))
	{
		xTimerStop(timer, 0);
		latest_response.response = AT_SUCCESS;
		postNewCommunicationEvent(EVENT_AT_REQUEST_COMPLETE, &latest_response);
		next_state = AT_STATE_IDLE;
	}
	else if(NULL != findLineInBuffer((const char *)reply_buffer, "ERROR"))
	{
		xTimerStop(timer, 0);
		latest_response.response = AT_FAILURE;
		postNewCommunicationEvent(EVENT_AT_REQUEST_COMPLETE, &latest_response);
		next_state = AT_STATE_IDLE;
	}
	return next_state;
}

ATState onAnyError(const void* data)
{
	latest_response.response = AT_FAILURE;
	postNewCommunicationEvent(EVENT_AT_REQUEST_COMPLETE, &latest_response);
	return AT_STATE_IDLE;
}

typedef ATState (*ATStateTransitionFunction)(const void* data);

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



void at_handleEvent(CommunicationEvent event)
{
	ATStateTransitionFunction function = at_state_transition_table[state][event.id];
	if(function != NULL)
	{
		state = function(event.data);
	}
}

