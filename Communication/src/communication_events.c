/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "communication_events.h"

#include "robotto_common.h"
#include "communication_sm_handlers.h"


const char* Communication_eventToString(CommunicationEventId event)
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



void handleCommunicationEvent(const CommunicationEvent* event)
{
	uart_rx_handleEvent(event);
	uart_tx_handleEvent(event);
	at_handleEvent(event);
	connection_handleEvent(event);
	telemetry_handleEvent(event);
}


