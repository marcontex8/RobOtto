/*
 * communication_manager.c
 *
 *  Created on: Nov 30, 2025
 *      Author: marco
 */

#include "robotto_common.h"
#include "network_communication.h"

#include "SEGGER_SYSVIEW.h"
#include <stddef.h>

extern bool UART_error;

static const char* last_error = "No error";

ActivityStatus runCommunicationManagerStatusInit()
{
	ActivityStatus next_activity_status = ACTIVITY_STATUS_ERROR;

	NetworkInitializationStatus status = initNetworkCommunication();
	if(NET_INIT_ERROR == status)
	{
		last_error = getError();
		next_activity_status = ACTIVITY_STATUS_ERROR;
	}
	else if(NET_INIT_SUCCESS == status)
	{
		next_activity_status = ACTIVITY_STATUS_RUNNING;
	}
	else
	{
		next_activity_status = ACTIVITY_STATUS_INIT;
	}
	return next_activity_status;
}

ActivityStatus runCommunicationManagerStatusRunning()
{
	SEGGER_SYSVIEW_Print("Running StatusRunning()");
	// check request from the network

	// list data to be sent




	return ACTIVITY_STATUS_RUNNING;
}


void runCommunicationManagerStateMachine()
{
	static ActivityStatus activity_status = ACTIVITY_STATUS_INIT;

	updateIncomingData();

	if(ACTIVITY_STATUS_INIT == activity_status)
	{
		activity_status = runCommunicationManagerStatusInit();
	}
	else if(ACTIVITY_STATUS_RUNNING == activity_status)
	{
		activity_status = runCommunicationManagerStatusRunning();
	}
	else // ACTIVITY_STATUS_ERROR
	{
		SEGGER_SYSVIEW_ErrorfTarget("ERROR STATE. Reason: %s\n", last_error);
	}
}
