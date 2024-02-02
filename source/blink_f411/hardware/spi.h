/*
 * spi.h
 *
 *  Created on: Jan 28, 2024
 *      Author: henry
 */

#ifndef SPI_H_
#define SPI_H_

#include <stddef.h>
#include <stdint.h>

#include "main.h"


void spi1_init(void);

void spi1_lcd_select(void);
void spi1_lcd_deselect(void);
void spi1_sdcard_select(void);
void spi1_sdcard_deselect(void);


uint8_t spi1_txByte(uint8_t data);
void spi1_lcd_txData(uint8_t* data, uint8_t length);



#endif /* SPI_H_ */
