/*
 * communication_manager.c
 *
 *  Created on: Nov 30, 2025
 *      Author: marco
 */

#include "robotto_common.h"
#include "communication_events.h"

#include "SEGGER_SYSVIEW.h"
#include "FreeRTOS.h"
#include "queue.h"

#include "robotto_conf.h"

void setCommunicationManagerQueue(QueueHandle_t communication_queue)
{
    setEventsQueue(communication_queue);
}

void runCommunicationManagerStateMachine()
{
	vTaskDelay(pdMS_TO_TICKS(NETWORK_RUN_DELAY_AT_STARTUP_S * 1000));
	postNewCommunicationEventWithNoData(EVENT_CONNECTION_INIT);
    for (;;)
    {
    	CommunicationEvent event;
        if (getNextCommunicationEvent(&event))
        {
    		//SEGGER_SYSVIEW_PrintfHost("Event: %s", eventToString(event.id));
    		handleCommunicationEvent(&event);
        }
    }
}
