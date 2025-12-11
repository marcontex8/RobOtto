/*
 * network_communication.c
 *
 *  Created on: Dec 1, 2025
 *      Author: marco
 */

#include "network_communication.h"
#include "robotto_common.h"
#include "uart_reader.h"
#include "at_state_machine.h"

#define MAX_COMMAND_REPETITIONS 3
/*
#ifndef ESP_RESET_Pin
  #define ESP_RESET_Pin GPIO_PIN_14
#endif
#ifndef ESP_RESET_GPIO_Port
  #define ESP_RESET_GPIO_Port GPIOC
#endif
*/
void activateESP()
{
	HAL_GPIO_WritePin(ESP_RESET_GPIO_Port, ESP_RESET_Pin, GPIO_PIN_SET);
}

static char* error_description = NULL;
/*
AtRequestStatus sendAT_CWMODE_1(AtAnswer* answer);
go to station mode
AT+CWMODE=1
>
>OK

connect to wifi
AT+CWJAP="SOPPALCO","scalaApioli!"
>WIFI CONNECTED
>WIFI GOT IP
>
>OK

AT+MQTTUSERCFG=0,1,"RobOTTO","","",0,0,""
>
>OK

AT+MQTTCONN=0,"192.168.1.140",1884,0
>+MQTTCONNECTED:0,1,"192.168.1.140","1884","",0
>
>OK



AT+MQTTPUB=0,"test/topic","hello from esp01",0,0
>
>OK
*/

static const char* init_commands[]={
		"",
		"AT",
		"AT+CWMODE=1",
		"AT+CWJAP=\"SOPPALCO\",\"scalaApioli!\"",
		"AT+MQTTUSERCFG=0,1,\"RobOTTO\",\"\",\"\",0,0,\"\"",
		"AT+MQTTCONN=0,\"192.168.1.140\",1884,0",
		"AT+MQTTPUB=0,\"test/topic\",\"hello from esp01\",0,0"
};

CommunicationStatus performInitStep(uint8_t* command_index)
{
	ATAnswer answer = {0};
	ATStatus request_status = ATSM_runRequest(init_commands[*command_index], &answer);
	if(AT_STATUS_DONE != request_status)
	{
		return COMM_STATUS_IN_PROGRESS;
	}

	if(AT_ANSWER_OK == answer)
	{
		++(*command_index);
		return COMM_STATUS_DONE;
	}

	static uint8_t counter = 0;
	if(++counter > MAX_COMMAND_REPETITIONS)
	{
		return COMM_STATUS_ERROR;
	}
	return COMM_STATUS_IN_PROGRESS;
}


CommunicationStatus initNetworkCommunication()
{
	static uint8_t command_index = 0;

	if(0 == command_index)
	{
		activateESP();
		ESP_UART_RxInit();
		++command_index;
		return COMM_STATUS_IN_PROGRESS;
	}
	else
	{
		CommunicationStatus init_status = performInitStep(&command_index);
		if(COMM_STATUS_ERROR == init_status)
		{
			error_description = init_commands[command_index];
		}
		return init_status;
	}
}


void parseNewData()
{
	ESP_UART_fetchAndParseNewData();
}

char* getError()
{
	return error_description;
}


