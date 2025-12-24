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
} UartRxState;


static UartRxState state = UART_RX_STATUS_OFF;
static UartRxData last_notified_data = {0};


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

	// TODO: this could be called again before the previous data has been processed.
	if (buffer_head > buffer_tail)
	{
		last_notified_data.first_chunk_start = &esp_uart_dma_rx_buf[buffer_tail];
		last_notified_data.first_chunk_size = buffer_head - buffer_tail;
	}
	else
	{
		last_notified_data.first_chunk_start = &esp_uart_dma_rx_buf[buffer_tail];
		last_notified_data.first_chunk_size = ESP_UART_DMA_RX_BUF_SIZE - buffer_tail;

		last_notified_data.second_chunk_start = &esp_uart_dma_rx_buf[0];
		last_notified_data.second_chunk_size = buffer_head;
	}

	postNewCommunicationEventFromISR(EVENT_UART_RX_NEW_DATA_RECEIVED, (void*)&last_notified_data);
	buffer_tail = buffer_head;
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart != &H_UART_ESP)
	{
		return;
	}

	postNewCommunicationEventFromISR(EVENT_UART_RX_ERROR, NULL);

}


UartRxState runUartRxStateOff(CommunicationEvent event)
{
	UartRxState next_state = UART_RX_STATUS_OFF;
	if(event.id == EVENT_UART_RX_START_REQUEST)
	{
		memset(esp_uart_dma_rx_buf, 0, ESP_UART_DMA_RX_BUF_SIZE);
		if(HAL_OK == HAL_UARTEx_ReceiveToIdle_DMA(&H_UART_ESP, esp_uart_dma_rx_buf, ESP_UART_DMA_RX_BUF_SIZE))
		{
			next_state = UART_RX_STATUS_LISTENING;
		}
		postNewCommunicationEvent(EVENT_UART_RX_STARTED, NULL);
	}
	return next_state;
}


UartRxState runUartRxStateListening(CommunicationEvent event)
{
	UartRxState next_state = UART_RX_STATUS_LISTENING;

	if(event.id == EVENT_UART_RX_ERROR)
	{
		next_state = UART_RX_STATUS_OFF;
		postNewCommunicationEvent(EVENT_UART_RX_START_REQUEST, NULL);
	}

	return next_state;
}


void uart_rx_handleEvent(CommunicationEvent event)
{
	switch(state)
	{
	case UART_RX_STATUS_OFF:
		state = runUartRxStateOff(event);
		break;
	case UART_RX_STATUS_LISTENING:
		state = runUartRxStateListening(event);
		break;
	default:
		// should never happen
	}
}


