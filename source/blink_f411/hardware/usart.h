/*
 * uart.h
 *
 *  Created on: Jan 28, 2024
 *      Author: henry
 */

#ifndef USART_H_
#define USART_H_

#include <stddef.h>
#include <stdint.h>
#include "main.h"

#define RX_BUFFER_SIZE    64
#define TX_BUFFER_SIZE    64


void usart2_init(void);
void usart2_txByte(uint8_t data);
void usart2_txData(uint8_t *data, uint8_t length);
void usart2_txString(const char* data);

void usart2_echo(uint8_t *data, uint8_t length);

void usart2_interrupt_handler(void);


#endif /* USART_H_ */
