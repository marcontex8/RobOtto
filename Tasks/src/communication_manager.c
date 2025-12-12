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


static const char* last_error = "No error";

ActivityStatus runCommunicationManagerStatusInit()
{
	SEGGER_SYSVIEW_Print("Running StatusInit()");

	NetworkInitializationStatus status = initNetworkCommunication();
	if(NET_INIT_IN_PROGRESS == status)
	{
		return ACTIVITY_STATUS_INIT;
	}
	else if(NET_INIT_SUCCESS == status)
	{
		return ACTIVITY_STATUS_RUNNING;
	}
	else
	{
		last_error = getError();
		return ACTIVITY_STATUS_ERROR;
	}
}

ActivityStatus runCommunicationManagerStatusRunning()
{
	SEGGER_SYSVIEW_Print("Running StatusRunning()");
	SEGGER_SYSVIEW_PrintfHost("Size of size_t %u", sizeof(size_t));
	SEGGER_SYSVIEW_PrintfHost("Size of int %u", sizeof(int));
	SEGGER_SYSVIEW_PrintfHost("Size of long int %u", sizeof(long int));
	SEGGER_SYSVIEW_PrintfHost("Size of short int %u", sizeof(short int));
	return ACTIVITY_STATUS_RUNNING;
}


void runCommunicationManagerStateMachine()
{
	static ActivityStatus activity_status = ACTIVITY_STATUS_INIT;

	SEGGER_SYSVIEW_Print("Before parseNewDataIfAny");
	parseNewDataIfAny();
	SEGGER_SYSVIEW_Print("After parseNewDataIfAny");

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
