/*
 * at_commands.c
 *
 *  Created on: Dec 2, 2025
 *      Author: marco
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "at_state_machine.h"
#include "uart_writer.h"
#include "robotto_conf.h"
#include "buffer_parser.h"
#include "robotto_common.h"

#include "SEGGER_SYSVIEW.h"

#include "FreeRTOS.h"
#include "task.h"


typedef enum{
	AT_STATUS_IDLE,
	AT_STATUS_WAITING,
	AT_STATUS_DONE,
} ATStatus;

/* REQUEST STATE VARIABLES */
static ATStatus at_status = AT_STATUS_IDLE;
static ATAnswer latest_answer = AT_ANSWER_ERROR;
static TickType_t start_request_tick;


/* REPLY BUFFER VARIABLES */
#define BUFFER_SIZE 1024
#define MAX_WRITABLE_BUFFER_SIZE (BUFFER_SIZE -1) // at least 1 termination char should be always present in the buffer
static uint8_t reply_buffer[BUFFER_SIZE];
static uint16_t reply_buffer_current_length = 0;


static uint16_t number_of_unexpected_communications = 0;


void ATSM_resetBuffer()
{
	memset(reply_buffer, 0, BUFFER_SIZE);
	reply_buffer_current_length = 0;
}


void ATSM_resetStateMachine()
{
	at_status = AT_STATUS_IDLE;
	ATSM_resetBuffer();
}

void startNewRequest(const char* command)
{
	ATSM_resetBuffer();

	if(ROBOTTO_OK != SendMessage(command))
	{
		latest_answer = AT_ANSWER_ERROR;
		at_status = AT_STATUS_DONE;
	}
	else
	{
		start_request_tick = xTaskGetTickCount();
		at_status = AT_STATUS_WAITING;
		latest_answer = AT_ANSWER_NONE;
	}
}

void completeRequest()
{
	// latest_answer already updated by the parser during parsing phase.
	at_status = AT_STATUS_IDLE;
	ATSM_resetBuffer();
}

void checkTimeout(const char* command)
{
	if( (xTaskGetTickCount() - start_request_tick) > pdMS_TO_TICKS(NETWORK_REQUEST_TIMEOUT_S * 1000))
	{
		SEGGER_SYSVIEW_PrintfHost("Timeout: %s", command);
		SEGGER_SYSVIEW_PrintfHost("Start time: %u | Now: %u", start_request_tick, xTaskGetTickCount());
		latest_answer = AT_ANSWER_ERROR;
		at_status = AT_STATUS_DONE;
	}
}

ATAnswer ATSM_runRequest(const char* command)
{
	if(AT_STATUS_IDLE == at_status)
	{
		SEGGER_SYSVIEW_PrintfHost("Start new request: %s", command);
		startNewRequest(command);
	}
	else if(AT_STATUS_DONE == at_status)
	{
		SEGGER_SYSVIEW_PrintfHost("Request Complete: %s", command);
		completeRequest();
	}
	else
	{
		checkTimeout(command);
	}
	return latest_answer;
}


//////////////////////////////////////////////////
//////////// CALLBACKS FROM IDLE UART ////////////
//////////////////////////////////////////////////

void ATSM_processNewData()
{
	if(AT_STATUS_WAITING != at_status)
	{
		SEGGER_SYSVIEW_Print("UNEXPECTED COMMUNICATION");
		++number_of_unexpected_communications;
	}
	else
	{
		if(NULL != findLineInBuffer((const char *)reply_buffer, "OK"))
		{
	    	latest_answer = AT_ANSWER_OK;
	    	at_status = AT_STATUS_DONE;
		}
		else if(NULL != findLineInBuffer((const char *)reply_buffer, "ERROR"))
		{
			latest_answer = AT_ANSWER_ERROR;
			at_status = AT_STATUS_DONE;
		}
		else
		{
			at_status = AT_STATUS_WAITING;
		}
	}

	// TODO: remove when debugging is over
	static char DEBUG_tmp_line[128];
	SYSVIEW_PrintLines((const char *)reply_buffer, DEBUG_tmp_line, 128);

}

void ATSM_newRecievedData(const uint8_t *data, uint16_t new_data_length)
{
	if(reply_buffer_current_length + new_data_length > MAX_WRITABLE_BUFFER_SIZE)
	{
		SEGGER_SYSVIEW_Warn("Received data overflow buffer size. Some data will be lost!");
		new_data_length = MAX_WRITABLE_BUFFER_SIZE - reply_buffer_current_length;
	}
	memcpy(&reply_buffer[reply_buffer_current_length], data, new_data_length);
	reply_buffer_current_length += new_data_length;
}

