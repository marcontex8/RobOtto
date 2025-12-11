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


static volatile ATStatus at_status = AT_STATUS_IDLE;
ATAnswer latest_answer = AT_ANSWER_ERROR;


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

// TODO add timeout and request id
ATStatus ATSM_runRequest(const char* command, ATAnswer* answer)
{
	if(AT_STATUS_IDLE == at_status)
	{
		ATSM_reset();
		if(ROBOTTO_OK != SendMessage(command))
		{
			latest_answer = AT_ANSWER_ERROR;
			at_status = AT_STATUS_DONE;
		}
		else
		{
			at_status = AT_STATUS_WAITING;
		}
		return at_status;
	}

	if(AT_STATUS_DONE == at_status)
	{
		at_status = AT_STATUS_IDLE;
		*answer = latest_answer;
		return AT_STATUS_DONE;
	}

	return at_status;
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
	    	latest_answer = AT_ANSWER_OK;
        	return AT_STATUS_DONE;
	    }
	    if (strcmp(line, "ERROR") == 0)
	    {
	    	latest_answer = AT_ANSWER_ERROR;
	        return AT_STATUS_DONE;
	    }
	}
	return AT_STATUS_WAITING;
}



void ATSM_processNewData()
{
	SEGGER_SYSVIEW_PrintfHost("%s", reply_buffer);

	if(AT_STATUS_WAITING != at_status)
	{
		++number_of_unexpected_communications;
	}
	else
	{
		at_status = parseReplyBuffer();
	}
}

void ATSM_newRecievedData(const uint8_t *data, uint16_t len)
{
	if(reply_len + len >= BUFFER_SIZE -1) // at least 1 termination char should be always present in the buffer
	{
		// TOD: it's very unlikely to happen, but proper solution should be implemented
    	latest_answer = AT_ANSWER_ERROR;
    	at_status = AT_STATUS_DONE;
	}
	memcpy(reply_buffer, data, len);
	reply_len += len;
}

