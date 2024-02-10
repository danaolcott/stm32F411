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


#define RCC_PLLM_MASK    ((uint32_t)0x0000003F)
#define RCC_PLLM_POS     0
#define RCC_PLLN_MASK    ((uint32_t)0x00007FC0)
#define RCC_PLLN_POS     6
#define RCC_PLLP_MASK    ((uint32_t)0x00030000)
#define RCC_PLLP_POS     16
#define RCC_PLLQ_MASK    ((uint32_t)0x0F000000)
#define RCC_PLLQ_POS     24


extern volatile uint32_t TimingDelay;
void Delay(volatile uint32_t timeTick);
void TimingDelay_Decrement(void);
void SystemClockConfig(uint32_t PLL_M, uint32_t PLL_N, uint32_t PLL_P, uint32_t PLL_Q);




#endif /* MAIN_H_ */
