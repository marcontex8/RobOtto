/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "FreeRTOS.h"
#include "task.h"

#include "communication_events.h"
#include "communication_queue.h"
#include "robotto_common.h"
#include "robotto_conf.h"

void Communication_task(void *argument)
{
    vTaskDelay(pdMS_TO_TICKS(NETWORK_RUN_DELAY_AT_STARTUP_S * 1000));
    triggerCommunicationInitialization();
    for (;;)
    {
        CommunicationEvent event;
        if (getNextCommunicationEvent(&event))
        {
            SEGGER_SYSVIEW_PrintfHost("Event: %s", Communication_eventToString(event.id));
            handleCommunicationEvent(&event);
        }
    }
}
