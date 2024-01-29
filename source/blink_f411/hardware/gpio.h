/*
 * gpio.h
 *
 *  Created on: Jan 27, 2024
 *      Author: henry
 */

#ifndef GPIO_H_
#define GPIO_H_


#include <stddef.h>
#include <stdint.h>

extern volatile uint8_t buttonFlag;

void gpio_init(void);
void gpio_button_init(void);
void gpio_button_handler(void);




#endif /* GPIO_H_ */
