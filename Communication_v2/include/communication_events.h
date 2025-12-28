/*
 * communication_events.h
 *
 *  Created on: Dec 18, 2025
 *      Author: marco
 */

#ifndef COMMUNICATION_EVENTS_H_
#define COMMUNICATION_EVENTS_H_

#include <stdint.h>
#include <stddef.h>

typedef enum{
	EVENT_UART_TX_REQUEST,
	EVENT_UART_TX_COMPLETE,
	EVENT_UART_TX_ERROR,

	EVENT_UART_RX_START_REQUEST,
	EVENT_UART_RX_STARTED,
	EVENT_UART_RX_ERROR,
	EVENT_UART_RX_NEW_DATA_RECEIVED,

	EVENT_AT_NEW_REQUEST,
	EVENT_AT_REQUEST_COMPLETE,
	EVENT_AT_REQUEST_TIMEOUT, // only AT internal, will trigger a REQUEWST_COMPLETE

	EVENT_COMM_INIT,
	EVENT_COMM_DELAY_EXPIRED, // only internal, used to delay the requests
	EVENT_COMM_REQUEST,


	EVENT_COUNT,
} CommunicationEventId;


const char* eventToString(CommunicationEventId event);



// data of EVENT_UART_TX_REQUEST
typedef struct{
	const uint8_t* buffer;
	size_t size;
} UartTxData;

/**
 * data of EVENT_UART_RX_NEW_DATA_RECEIVED
 * DMA data are stored in a circular buffer.
 * If the buffer wraps around there could be a first chuck until the end
 * and a second one starting at the beginning of it.
 */
typedef struct{
	const uint8_t* first_chunk_start;
	size_t first_chunk_size;
	const uint8_t* second_chunk_start;
	size_t second_chunk_size;
} UartRxData;


// data of EVENT_AT_NEW_REQUEST
typedef struct{
	unsigned int request_id;
	const char* buffer;
} ATRequestData;



typedef enum{
	AT_SUCCESS,
	AT_FAILURE,
} ATResponseResult;

// data of EVENT_AT_REQUEST_COMPLETE
typedef struct{
	unsigned int request_id; //TODO: at the moment this is not populated
	ATResponseResult response;
} ATResponseData;


typedef union{
	void* pointer;
	UartTxData uart_tx;
	UartRxData uart_rx;
	ATRequestData at_request;
	ATResponseData at_response;
} CommunicationEventData;


typedef struct{
	const CommunicationEventId id;
	const CommunicationEventData data;
} CommunicationEvent;

void postNewCommunicationEvent(CommunicationEventId event_id, CommunicationEventData data);
void postNewCommunicationEventWithNoData(CommunicationEventId event_id);
void postNewCommunicationEventFromISR(CommunicationEventId event_id, CommunicationEventData data);
void postNewCommunicationEventFromISRWithNoData(CommunicationEventId event_id);


void at_handleEvent(const CommunicationEvent* event);
void communication_handleEvent(const CommunicationEvent* event);
void uart_rx_handleEvent(const CommunicationEvent* event);
void uart_tx_handleEvent(const CommunicationEvent* event);


#endif /* COMMUNICATION_EVENTS_H_ */
