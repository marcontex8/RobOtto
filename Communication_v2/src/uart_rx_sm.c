/*
 * uart_rx_sm.c
 *
 *  Created on: Dec 18, 2025
 *      Author: marco
 */

#include "communication_events.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal_uart.h"

#include <string.h>

#define H_UART_ESP huart4
#define ESP_UART_DMA_RX_BUF_SIZE 1024

extern UART_HandleTypeDef huart4;

uint8_t esp_uart_dma_rx_buf[ESP_UART_DMA_RX_BUF_SIZE];
static size_t buffer_tail = 0;

typedef enum{
	UART_RX_STATUS_OFF,
	UART_RX_STATUS_LISTENING,

	UART_RX_STATUS_COUNT,
} UartRxState;


static UartRxState state = UART_RX_STATUS_OFF;


void UART4_IRQHandler()
{
    HAL_UART_IRQHandler(&H_UART_ESP);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart != &H_UART_ESP)
	{
		return;
	}

	uint16_t buffer_head = ESP_UART_DMA_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(H_UART_ESP.hdmarx);

	if (buffer_head == buffer_tail)
	{
		return;
	}

	UartRxData uart_rx_data = {0};
	if (buffer_head > buffer_tail)
	{
		uart_rx_data.first_chunk_start = &esp_uart_dma_rx_buf[buffer_tail];
		uart_rx_data.first_chunk_size = buffer_head - buffer_tail;
	}
	else
	{
		uart_rx_data.first_chunk_start = &esp_uart_dma_rx_buf[buffer_tail];
		uart_rx_data.first_chunk_size = ESP_UART_DMA_RX_BUF_SIZE - buffer_tail;

		uart_rx_data.second_chunk_start = &esp_uart_dma_rx_buf[0];
		uart_rx_data.second_chunk_size = buffer_head;
	}



	CommunicationEventData data_to_send = {.uart_rx = uart_rx_data};
	postNewCommunicationEventFromISR(EVENT_UART_RX_NEW_DATA_RECEIVED, data_to_send);
	buffer_tail = buffer_head;
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart != &H_UART_ESP)
	{
		return;
	}

	postNewCommunicationEventFromISRWithNoData(EVENT_UART_RX_ERROR);

}


UartRxState onRxStartRequest(const CommunicationEventData*)
{
	UartRxState next_state = UART_RX_STATUS_OFF;

	memset(esp_uart_dma_rx_buf, 0, ESP_UART_DMA_RX_BUF_SIZE);
	if(HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&H_UART_ESP, esp_uart_dma_rx_buf, ESP_UART_DMA_RX_BUF_SIZE))
	{
		next_state = UART_RX_STATUS_LISTENING;
		postNewCommunicationEventWithNoData(EVENT_UART_RX_STARTED);
	}
	else
	{
		next_state = UART_RX_STATUS_OFF;
		postNewCommunicationEventWithNoData(EVENT_UART_RX_START_REQUEST);
	}
	return next_state;
}


UartRxState onUartRxError(const CommunicationEventData*)
{
	UartRxState next_state = UART_RX_STATUS_LISTENING;

	next_state = UART_RX_STATUS_OFF;
	postNewCommunicationEventWithNoData(EVENT_UART_RX_START_REQUEST);

	return next_state;
}


typedef UartRxState (*UartRxTransitionFunction)(const CommunicationEventData* data);

static const UartRxTransitionFunction uart_rx_state_transition_table[UART_RX_STATUS_COUNT][EVENT_COUNT] = {
    [UART_RX_STATUS_OFF] = {
        [EVENT_UART_RX_START_REQUEST] = onRxStartRequest,
    },
    [UART_RX_STATUS_LISTENING] = {
        [EVENT_UART_RX_ERROR] = onUartRxError,
    },
};

void uart_rx_handleEvent(const CommunicationEvent* event)
{
	UartRxTransitionFunction function = uart_rx_state_transition_table[state][event->id];
	if(function != NULL)
	{
		state = function(&(event->data));
	}
}

