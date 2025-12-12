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
#define REQUEST_TIMEOUT_SECONDS 10
static TickType_t start_request_tick;


/* REPLY BUFFER VARIABLES */
#define BUFFER_SIZE 1024
static uint8_t reply_buffer[BUFFER_SIZE];
static uint16_t reply_len = 0;


static uint16_t number_of_unexpected_communications = 0;


void ATSM_reset()
{
	memset(reply_buffer, 0, BUFFER_SIZE);
	reply_len = 0;
	latest_answer = AT_ANSWER_ERROR;
}


void startNewRequest(const char* command)
{
	ATSM_reset();
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
}

void checkTimeout()
{
	if( (xTaskGetTickCount() - start_request_tick) > pdMS_TO_TICKS(REQUEST_TIMEOUT_SECONDS * 1000))
	{
		latest_answer = AT_ANSWER_ERROR;
		at_status = AT_STATUS_DONE;
	}
}

ATAnswer ATSM_runRequest(const char* command)
{
	if(AT_STATUS_IDLE == at_status)
	{
		startNewRequest(command);
	}
	else if(AT_STATUS_DONE == at_status)
	{
		completeRequest();
	}
	else
	{
		checkTimeout();
	}
	return latest_answer;
}


//////////////////////////////////////////////////
//////////// CALLBACKS FROM IDLE UART ////////////
//////////////////////////////////////////////////

ATStatus parseReplyBuffer()
{
	const char *p = (const char *)reply_buffer;
	char line[128];
	while (*p != '\0')
	{
		// extract one line (until \r or \n)
	    int i = 0;
	    while (*p != '\0' && *p != '\n' && *p != '\r' && i < (int)(sizeof(line)-1)) {
	    	line[i++] = *p++;
	    }
	    line[i] = '\0';

	    // skip CR/LF
	    while (*p == '\r' || *p == '\n')
	    {
	    	p++;
	    }

	    // ignore empty lines
	    if (i == 0)
	    {
	    	continue;
	    }

	    // check minimal match
	    if (strcmp(line, "OK") == 0)
	    {
			SEGGER_SYSVIEW_Print("Found line OK, setting latest_answer");
	    	latest_answer = AT_ANSWER_OK;
        	return AT_STATUS_DONE;
	    }
	    if (strcmp(line, "ERROR") == 0)
	    {
			SEGGER_SYSVIEW_Print("Found line ERROR, setting latest_answer");
	    	latest_answer = AT_ANSWER_ERROR;
	        return AT_STATUS_DONE;
	    }
	}
	return AT_STATUS_WAITING;
}



void ATSM_processNewData()
{
	SEGGER_SYSVIEW_PrintfHost("Process new data: %s", reply_buffer);

	if(AT_STATUS_WAITING != at_status)
	{
		SEGGER_SYSVIEW_Print("unexpected");
		++number_of_unexpected_communications;
	}
	else
	{
		SEGGER_SYSVIEW_Print("expected");
		at_status = parseReplyBuffer();
	}
}

void ATSM_newRecievedData(const uint8_t *data, uint16_t len)
{
	if(reply_len + len >= BUFFER_SIZE -1) // at least 1 termination char should be always present in the buffer
	{
		// TODO: It happens constantly at the startup. A proper solution should be implemented
		return;
	}
	memcpy(reply_buffer, data, len);
	reply_len += len;
}

