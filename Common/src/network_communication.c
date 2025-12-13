/*
 * network_communication.c
 *
 *  Created on: Dec 1, 2025
 *      Author: marco
 */

#include "network_communication.h"
#include "robotto_common.h"
#include "robotto_conf.h"
#include "uart_reader.h"
#include "at_state_machine.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#define MAX_COMMAND_REPETITIONS 3

#define ESP_RESET_Pin GPIO_PIN_14
#define ESP_RESET_GPIO_Port GPIOC

void activateESP()
{
	HAL_GPIO_WritePin(ESP_RESET_GPIO_Port, ESP_RESET_Pin, GPIO_PIN_SET);
}

static const char* error_description = NULL;

static NetworkInitializationStatus network_initialization_status = NET_INIT_IDLE;

static const char* init_commands[]={
		"AT",
		"ATE0",
		"AT+CWMODE=1",
		"AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PWD "\"",
		"AT+MQTTUSERCFG=0,1,\"RobOTTO\",\"\",\"\",0,0,\"\"",
		"AT+MQTTCONN=0,\"192.168.1.140\",1884,0",
		"AT+MQTTPUB=0,\"test/topic\",\"hello from esp01\",0,0"
};

static const size_t NUMBER_OF_INITIALIZATION_STEPS =
    sizeof(init_commands) / sizeof(init_commands[0]);


NetworkInitializationStatus performInitSteps()
{
	static uint8_t current_command_index = 0;
	static uint8_t command_repetition = 0;

	NetworkInitializationStatus next_status = NET_INIT_IN_PROGRESS;

	SEGGER_SYSVIEW_PrintfHost("Initialization command index %u", current_command_index);
	if(current_command_index >= NUMBER_OF_INITIALIZATION_STEPS)
	{
		next_status = NET_INIT_SUCCESS;
	}
	else
	{
		ATAnswer answer = ATSM_runRequest(init_commands[current_command_index]);
		if(AT_ANSWER_OK == answer)
		{
			SEGGER_SYSVIEW_Print("Request Complete OK");
			++current_command_index;
			command_repetition = 0;
		}
		else if(AT_ANSWER_ERROR == answer)
		{
			SEGGER_SYSVIEW_Print("Request Complete ERROR");
			if(command_repetition < MAX_COMMAND_REPETITIONS)
			{
				++command_repetition;
			}
			else
			{
				error_description = init_commands[current_command_index];
				next_status = NET_INIT_ERROR;
			}
		}
	}
	return next_status;
}


NetworkInitializationStatus initNetworkCommunication()
{
	if(NET_INIT_IDLE == network_initialization_status)
	{
		SEGGER_SYSVIEW_Print("Activate UART");
		ESP_UART_RxInit();
		network_initialization_status = NET_INIT_IN_PROGRESS;
	}
	else if(NET_INIT_IN_PROGRESS == network_initialization_status)
	{
		network_initialization_status = performInitSteps();
	}
	return network_initialization_status;
}


void parseNewDataIfAny()
{
	// fetch data only after DMA is initialized correctly
	if(NET_INIT_IDLE != network_initialization_status)
	{
		ESP_UART_fetchAndParseNewData();
	}
}

const char* getError()
{
	return error_description;
}


