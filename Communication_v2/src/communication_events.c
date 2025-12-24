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
        case EVENT_UART_TX_REQUEST:   return "EVENT_UART_TX_REQUEST";
        case EVENT_UART_TX_COMPLETE: return "EVENT_UART_TX_COMPLETE";
        case EVENT_UART_TX_ERROR:  return "EVENT_UART_TX_ERROR";
        case EVENT_UART_RX_START_REQUEST:  return "EVENT_UART_RX_START_REQUEST";
        case EVENT_UART_RX_STARTED:  return "EVENT_UART_RX_STARTED";
        case EVENT_UART_RX_ERROR:  return "EVENT_UART_RX_ERROR";
        case EVENT_UART_RX_NEW_DATA_RECEIVED:  return "EVENT_UART_RX_NEW_DATA_RECEIVED";
        case EVENT_AT_NEW_REQUEST:  return "EVENT_AT_NEW_REQUEST";
        case EVENT_AT_REQUEST_COMPLETE:  return "EVENT_AT_REQUEST_COMPLETE";
        case EVENT_AT_REQUEST_TIMEOUT:  return "EVENT_AT_REQUEST_TIMEOUT";
        case EVENT_COMM_INIT:  return "EVENT_COMM_INIT";
        case EVENT_COMM_REQUEST:  return "EVENT_COMM_REQUEST";
        default:          return "UNKNOWN";
    }
}

void postNewCommunicationEvent(CommunicationEventId event_id, const void* data)
{
	CommunicationEvent event = {.data = data, .id = event_id};
	xQueueSendToBack(robotto_communication_queue_handle, &event, 0);
}

void postNewCommunicationEventFromISR(CommunicationEventId event_id, const void* data)
{
	CommunicationEvent event = {.data = data, .id = event_id};
	xQueueSendToBackFromISR(robotto_communication_queue_handle, &event, 0);
}


