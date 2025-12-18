/*
 * uart_reader.h
 *
 *  Created on: Dec 5, 2025
 *      Author: marco
 */

#ifndef INCLUDE_UART_READER_H_
#define INCLUDE_UART_READER_H_

#include <stdbool.h>

void ESP_UART_RxInit();

bool ESP_UART_ErrorDetected();

void ESP_UART_fetchAndParseNewData();


#endif /* INCLUDE_UART_READER_H_ */
