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

extern QueueHandle_t robotto_communication_queue_handle;

void runCommunicationManagerStateMachine()
{
	vTaskDelay(pdMS_TO_TICKS(NETWORK_RUN_DELAY_AT_STARTUP_S * 1000));
	postNewCommunicationEventWithNoData(EVENT_COMM_INIT);
    for (;;)
    {
    	CommunicationEvent event;
        if (xQueueReceive(robotto_communication_queue_handle, &event, portMAX_DELAY))
        {
    		SEGGER_SYSVIEW_PrintfHost("Event: %s", eventToString(event.id));
        	uart_rx_handleEvent(&event);
        	uart_tx_handleEvent(&event);
        	at_handleEvent(&event);
        	communication_handleEvent(&event);
        }
    }
}
