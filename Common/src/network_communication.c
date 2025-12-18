/*
 * network_communication.c
 *
 *  Created on: Dec 1, 2025
 *      Author: marco
 */

#include "network_communication.h"
#include "robotto_common.h"
#include "robotto_conf.h"
#include "at_state_machine.h"
#include "uart_reader.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "main.h"

static TickType_t initialization_start_tick;

void activateESP()
{
	HAL_GPIO_WritePin(ESP_RESET_GPIO_Port, ESP_RESET_Pin, GPIO_PIN_SET);
}

static const char* error_description = NULL;

static NetworkInitializationStatus network_initialization_status = NET_INIT_IDLE;


/**
 * Other useful commands
		"AT+CWLAP", // list access points
		"AT+RESTORE", // restore to factory settings
 */
static const char* init_commands[] = {
		"AT",
		"ATE1",
		"AT+CWMODE=1",
		"AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PWD "\"",
		"AT+MQTTUSERCFG=0,1,\"RobOTTO\",\"\",\"\",0,0,\"\"",
		"AT+MQTTCONN=0,\"192.168.1.140\",1884,0",
		"AT+MQTTPUB=0,\"test/topic\",\"Hello from RobOtto\",0,0"
};

static const size_t NUMBER_OF_INITIALIZATION_STEPS =
    sizeof(init_commands) / sizeof(init_commands[0]);


bool delayIsNeeded(TickType_t previous_command_complention_tick, uint8_t current_command_index)
{
	if(current_command_index == 0)
	{
		return false;
	}
	if( (xTaskGetTickCount() - previous_command_complention_tick) > pdMS_TO_TICKS(1000))
	{
		return false;
	}
	return true;
}

NetworkInitializationStatus performFullInitSteps()
{
	static TickType_t previous_command_complention_tick = 0;
	static uint8_t current_command_index = 0;
	static uint8_t command_repetition = 0;

	NetworkInitializationStatus next_status = NET_INIT_STEPS_PROGRESS;

	if(current_command_index >= NUMBER_OF_INITIALIZATION_STEPS)
	{
		next_status = NET_INIT_SUCCESS;
	}
	else if(delayIsNeeded(previous_command_complention_tick, current_command_index))
	{
		// skip the step logic
	}
	else
	{
		ATAnswer answer = ATSM_runRequest(init_commands[current_command_index]);
		if(AT_ANSWER_OK == answer)
		{
			SEGGER_SYSVIEW_Print("Request Complete OK");
			++current_command_index;
			previous_command_complention_tick = xTaskGetTickCount();
			SEGGER_SYSVIEW_PrintfHost("Next step: %u", current_command_index);
			command_repetition = 0;
		}
		else if(AT_ANSWER_ERROR == answer)
		{
			if(command_repetition < NETWORK_COMMANDS_MAX_REPETITIONS)
			{
				SEGGER_SYSVIEW_Print("Request Complete with error, RETRY");
				++command_repetition;
			}
			else
			{
				SEGGER_SYSVIEW_Print("Request Complete with error, STOP.");
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
		activateESP(); // TODO check if it's actually working or not
		initialization_start_tick = xTaskGetTickCount();

		network_initialization_status = NET_INIT_WAITING_ESP_INIT;
	}
	else if(NET_INIT_WAITING_ESP_INIT == network_initialization_status)
	{
		if( (xTaskGetTickCount() - initialization_start_tick) > pdMS_TO_TICKS(NETWORK_RUN_DELAY_AT_STARTUP_S * 1000))
		{
			network_initialization_status = NET_INIT_STEPS_PROGRESS;
			ESP_UART_RxInit();
		}
	}
	else if(NET_INIT_STEPS_PROGRESS == network_initialization_status)
	{
		network_initialization_status = performFullInitSteps();
	}
	return network_initialization_status;
}


void updateIncomingData()
{
	// if initialization is not started there is no need to check status and data
	if(NET_INIT_IDLE != network_initialization_status)
	{
		if(ESP_UART_ErrorDetected())
		{
			// TODO: implement a detection for recurring uart error could be implemented
			SEGGER_SYSVIEW_Error("An error from UART peripheral was detected. Re-initilizing.");

			ATSM_resetStateMachine();

			ESP_UART_RxInit();
		}
		else
		{
			ESP_UART_fetchAndParseNewData();
		}
	}
}

const char* getError()
{
	return error_description;
}


