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


static char* last_error = NULL;

ActivityStatus runCommunicationManagerStatusInit()
{
	CommunicationStatus status = initNetworkCommunication();
	if(COMM_STATUS_IN_PROGRESS == status)
	{
		return ACTIVITY_STATUS_INIT;
	}

	if(COMM_STATUS_DONE != status)
	{
		last_error = getError();
		return ACTIVITY_STATUS_ERROR;
	}
	return ACTIVITY_STATUS_RUNNING;
}

ActivityStatus runCommunicationManagerStatusRunning()
{
	return ACTIVITY_STATUS_RUNNING;
}


void runCommunicationManagerStateMachine()
{
	static ActivityStatus activity_status = ACTIVITY_STATUS_INIT;
	parseNewData();

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
		SEGGER_SYSVIEW_ErrorfTarget("%s\n", last_error);
	}
}
