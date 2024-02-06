/*
 * utility.h
 *
 *  Created on: Feb 5, 2024
 *      Author: henry
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <stddef.h>
#include <stdint.h>

void utility_parseArgs(char *in, uint8_t *pargc, char **argv, uint8_t bufferSize);
uint8_t utility_decimalToBuffer(uint16_t val, uint8_t* buffer, uint8_t size);
uint8_t utility_decimalToBinary(uint16_t val, uint8_t* buffer, uint8_t size);


#endif /* UTILITY_H_ */
