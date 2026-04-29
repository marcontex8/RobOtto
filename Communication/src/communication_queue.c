/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "communication_queue.h"
#include "communication_events.h"

static QueueHandle_t comm_events_queue = NULL;

RobottoErrorCode initializeQueue()
{
    comm_events_queue = xQueueCreate(100, sizeof(CommunicationEvent));
    if (comm_events_queue == NULL)
    {
        return ROBOTTO_ERROR;
    }
    return ROBOTTO_OK;
}

static QueueHandle_t getEventsQueue()
{
    ROBOTTO_ASSERT_DEBUG(comm_events_queue != NULL);
    return comm_events_queue;
}


void postNewCommunicationEvent(CommunicationEventId event_id, CommunicationEventData data)
{
	CommunicationEvent event = {.data = data, .id = event_id};
	xQueueSendToBack(getEventsQueue(), &event, 0);
}

void postNewCommunicationEventWithNoData(CommunicationEventId event_id)
{
	CommunicationEventData empty_data = {NULL};
	postNewCommunicationEvent(event_id, empty_data);
}

void postNewCommunicationEventFromISR(CommunicationEventId event_id, CommunicationEventData data)
{
	CommunicationEvent event = {.data = data, .id = event_id};
	xQueueSendToBackFromISR(getEventsQueue(), &event, 0);
}

void postNewCommunicationEventFromISRWithNoData(CommunicationEventId event_id)
{
	CommunicationEventData empty_data = {NULL};
	postNewCommunicationEventFromISR(event_id, empty_data);
}

void triggerCommunicationInitialization()
{
    postNewCommunicationEventWithNoData(EVENT_CONNECTION_INIT);
}

bool getNextCommunicationEvent(CommunicationEvent* event)
{
    return xQueueReceive(getEventsQueue(), event, portMAX_DELAY) == pdTRUE;
}
