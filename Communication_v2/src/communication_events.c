/*
 * communication_events.c
 *
 *  Created on: Dec 19, 2025
 *      Author: marco
 */


#include "communication_events.h"
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t robotto_communication_queue_handle;

const char* eventToString(CommunicationEventId event)
{
    switch (event) {
        case EVENT_UART_TX_REQUEST:   return "UART_TX_REQUEST";
        case EVENT_UART_TX_COMPLETE: return "UART_TX_COMPLETE";
        case EVENT_UART_TX_ERROR:  return "UART_TX_ERROR";
        case EVENT_UART_RX_START_REQUEST:  return "UART_RX_START_REQUEST";
        case EVENT_UART_RX_STARTED:  return "UART_RX_STARTED";
        case EVENT_UART_RX_ERROR:  return "UART_RX_ERROR";
        case EVENT_UART_RX_NEW_DATA_RECEIVED:  return "UART_RX_NEW_DATA_RECEIVED";
        case EVENT_AT_NEW_REQUEST:  return "AT_NEW_REQUEST";
        case EVENT_AT_REQUEST_COMPLETE:  return "AT_REQUEST_COMPLETE";
        case EVENT_AT_REQUEST_TIMEOUT:  return "AT_REQUEST_TIMEOUT";
        case EVENT_CONNECTION_INIT:  return "CONNECTION_INIT";
        case EVENT_CONNECTION_DELAY_EXPIRED: return "CONNECTION_DELAY_EXPIRED";
        case EVENT_CONNECTION_ESTABLISHED: return "CONNECTION_ESTABLISHED";
        case EVENT_TELEMETRY_TICK: return "TELEMETRY_TICK";
        default:          return "UNKNOWN";
    }
}

void postNewCommunicationEvent(CommunicationEventId event_id, CommunicationEventData data)
{
	CommunicationEvent event = {.data = data, .id = event_id};
	xQueueSendToBack(robotto_communication_queue_handle, &event, 0);
}

void postNewCommunicationEventWithNoData(CommunicationEventId event_id)
{
	CommunicationEventData empty_data = {NULL};
	postNewCommunicationEvent(event_id, empty_data);
}

void postNewCommunicationEventFromISR(CommunicationEventId event_id, CommunicationEventData data)
{
	CommunicationEvent event = {.data = data, .id = event_id};
	xQueueSendToBackFromISR(robotto_communication_queue_handle, &event, 0);
}

void postNewCommunicationEventFromISRWithNoData(CommunicationEventId event_id)
{
	CommunicationEventData empty_data = {NULL};
	postNewCommunicationEventFromISR(event_id, empty_data);
}


void handleCommunicationEvent(const CommunicationEvent* event)
{
	uart_rx_handleEvent(event);
	uart_tx_handleEvent(event);
	at_handleEvent(event);
	connection_handleEvent(event);
	telemetry_handleEvent(event);
}


