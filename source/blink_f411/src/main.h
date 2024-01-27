/*
 * main.h
 *
 *  Created on: Jan 27, 2024
 *      Author: henry
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_it.h"


extern volatile uint32_t TimingDelay;
void Delay(volatile uint32_t timeTick);
void TimingDelay_Decrement(void);


void gpio_init(void);
void gpio_button_init(void);

void gpio_button_handler(void);

#endif /* MAIN_H_ */
