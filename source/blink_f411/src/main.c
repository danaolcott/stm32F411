/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2024-01-27

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "main.h"


///////////////////////////////////////////
//globals
volatile uint32_t TimingDelay;

char buffer[64] = {0x00};
int size = 0;

int main(void)
{
    //Configure the system clock as default values
    //with the PLL on using HSI clock.  Results
    //in system core clock running at 96mhz
    //peripheral bus speeds at 24mhz and 48mhz
    SystemClockConfig();

    if (SysTick_Config(SystemCoreClock / 1000))
    {
        while (1);
    }

    //configure the hardware
    gpio_init();
    gpio_button_init();
    adc_init();
    usart2_init();
    spi1_init();
    timer2_init(TIMER_SPEED_8KHZ);
    wav_init();

    LCD_Config();
    LCD_BacklightOn();
    LCD_Clear(0x00);

    //returns the status of the sdcard
    //value > 0 - Size of the directory file after it's built
    //value < 0 - negative value of the FR_RESULT code

    LCD_DrawStringKern(0, 3, "Init SDCard...");
    int result = SD_Init();

    if (result > 0)
    {
        size = snprintf(buffer, 32, "SD Res: %d", result);
        LCD_DrawStringKernLength(1, 3, (uint8_t*)buffer, size);
        size = snprintf(buffer, 32, "Dir Size: %d", result);
        LCD_DrawStringKernLength(2, 3, (uint8_t*)buffer, size);
    }
    else
    {
        //invalid / problem with initializing the sdcard
        LCD_DrawStringKern(1, 3, "SDCard Error...");
        size = snprintf(buffer, 32, "SD Res: %d", (-1)*result);
        LCD_DrawStringKernLength(2, 3, (uint8_t*)buffer, size);
    }


    /* Infinite loop */
    while (1)
    {
        //only do something with the LCD if not accessing
        //the file.
        if (!wav_getSoundPlayStatus())
        {
            LCD_Clear(0x00);
            LCD_DrawStringKern(0, 3, "Hell0 LCD");

            //output the adc from the dma buffer and the register directly
            size = snprintf(buffer, 32, "ADC1: %d", adc_getValueFromDMA());
            LCD_DrawStringKernLength(2, 3, (uint8_t*)buffer, size);

            size = snprintf(buffer, 32, "ADC-DR: %d", (uint16_t)ADC1->DR);
            LCD_DrawStringKernLength(3, 3, (uint8_t*)buffer, size);

            size = snprintf(buffer, 32, "OverRun: %d", adc_getError());
            LCD_DrawStringKernLength(4, 3, (uint8_t*)buffer, size);

            timer2_stop();
            gpio_shieldLedToggle();
        }

        //get the button status...
        if (buttonFlag == 1)
        {
            wav_playSound("RING.WAV");
            buttonFlag = 0;
        }


        Delay(1000);

    }
}



/////////////////////////////////////////////////
//Delay
void Delay(volatile uint32_t timeTick)
{
    TimingDelay = timeTick;
    while (TimingDelay != 0);
}


////////////////////////////////////////////////
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0)
        TimingDelay--;
}


/////////////////////////////////////////////////
//Configures the system core clock to run with the
//PLL using default M, N, P, Q values.  Results
//in System core clock = 96mhz
//
//Uses the following prescalers:
//AHB Prescaler 1
//APB1 Prescaler 4
//APB2 Prescaler 2
//
void SystemClockConfig()
{
    //enable the power clock
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    waste_cpu(10);

    //enable the HSI clock - this is default
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    //set the HSI clock as the main clock
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_SW)) | RCC_CFGR_SW_HSI;

    //make sure the PLL is off - this is default
    RCC->CR &= ~RCC_CR_PLLON;

    //set the power control register - VOS bits
    //to max cpu speed < 100hz.  default is < 84mhz
    PWR->CR |= PWR_CR_VOS;
    waste_cpu(10);

    //enable the PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
    waste_cpu(10);

    //set the flash acr reg = 3 waitstates - reset value = 0x00
    FLASH->ACR |= FLASH_ACR_LATENCY_3WS;

    //configure AHB/APB clocks - reset value for CFGR = 0x00
    RCC->CFGR &= ~RCC_CFGR_HPRE_DIV1; //AHB Prescaler 1
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4; //APB1 Prescaler 4
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; //APB2 Prescaler 2

    //set the PLL as the main clock
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_SW)) | RCC_CFGR_SW_PLL;

    //wait while the PLL is used as the system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_PLL));

    //finally, update the system core clock.
    //should read 96mhz where before it read as 16mhz
    SystemCoreClockUpdate();
}


void waste_cpu(uint32_t delay)
{
    volatile uint32_t temp = delay;
    while (temp > 0)
        temp--;
}

