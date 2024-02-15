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

#include "hardware.h"
#include "gpio.h"
#include "usart.h"
#include "spi.h"
#include "adc.h"
#include "timer.h"
#include "lcd_12864_dfrobot.h"
#include "sdcard_driver.h"
#include "cli.h"
#include "utility.h"
#include "wav.h"
#include "playlist.h"


extern volatile uint32_t TimingDelay;
void Delay(volatile uint32_t timeTick);
void TimingDelay_Decrement(void);
void SystemClockConfig();
void waste_cpu(uint32_t delay);




#endif /* MAIN_H_ */
