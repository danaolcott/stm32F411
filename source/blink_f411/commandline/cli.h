/*
 * cli.h
 *
 *  Created on: Feb 5, 2024
 *      Author: henry
 */

#ifndef CLI_H_
#define CLI_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

void cli_processBuffer(uint8_t* data, uint8_t length);
void cli_parseArgs(char *in, uint8_t *pargc, char **argv, uint8_t bufferSize);
uint8_t cli_decimalToBuffer(uint16_t val, uint8_t* buffer, uint8_t size);


#endif /* CLI_H_ */
