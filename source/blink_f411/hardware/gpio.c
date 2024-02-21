/*
 * gpio.c
 *
 *  Created on: Jan 27, 2024
 *      Author: henry
 */
#include <stddef.h>
#include <stdint.h>

#include "main.h"
#include "gpio.h"

volatile uint8_t buttonFlag;

///////////////////////////////////////////
//Initialize all the pins for leds, control lines
//for the display.... others?
//PA13 - this is shared with led on the board
//and the spi1 sck pin, so no need to initialize.
//Control lines for the LCD
////lcd controls
//#define LCD_CMD_Pin GPIO_Pin_7
//#define LCD_CMD_GPIO_Port GPIOC

//#define LCD_Backlight_Pin GPIO_Pin_8
//#define LCD_Backlight_GPIO_Port GPIOA
//#define LCD_Reset_Pin GPIO_Pin_9
//#define LCD_Reset_GPIO_Port GPIOA
//
//PC7, PA8 and PA9

//Configure alternate pin for cs for LCD - PB10
//this maps to D6 on arduino pinout.  To use the
//display going forwared, use PB10, and cut the pin
//on D10 (PB6) since this will be shared with the sdcard
//include PB0 for shield LED
//
//configure the 5 bit dac pins for sound output
////PB13, PB14, PB15, PB1, PB2
void gpio_init(void)
{
    //init structs
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);   //port A clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);   //port C clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);   //port A clocks

    //PA8 and PA9 - backlight and reset pins
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //PC7 - LCS CMD Pin
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //PB0 - shieldLed Pin
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    ////////////////////////////////////////////////
    //DAC output pins for new sound board
    //PB 13,14,15, 1, 2, 12 - bits 0-5
    GPIO_InitStructure.GPIO_Pin =  DAC_Bit0_Pin | DAC_Bit1_Pin | DAC_Bit2_Pin | DAC_Bit3_Pin | DAC_Bit4_Pin | DAC_Bit5_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //DAC output pins on PORTA - PA11, PA12 - bits 6 and 7
    GPIO_InitStructure.GPIO_Pin =  DAC_Bit6_Pin | DAC_Bit7_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    ////////////////////////////////////////////////////
    //OLD BOARD
    //DAC output pins for sound
    //bit 0 is on port A, bits 1-6 is on port B
//    GPIO_InitStructure.GPIO_Pin =  DAC_Bit1_Pin | DAC_Bit2_Pin | DAC_Bit3_Pin | DAC_Bit4_Pin | DAC_Bit5_Pin | DAC_Bit6_Pin;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //dac bit 0 is on port A, PA11
//    GPIO_InitStructure.GPIO_Pin =  DAC_Bit0_Pin;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
//    GPIO_Init(GPIOA, &GPIO_InitStructure);


    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    GPIO_ResetBits(GPIOA, GPIO_Pin_9);
    GPIO_ResetBits(GPIOC, GPIO_Pin_7);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    GPIO_ResetBits(GPIOB, DAC_Bit0_Pin | DAC_Bit1_Pin | DAC_Bit2_Pin | DAC_Bit3_Pin | DAC_Bit4_Pin | DAC_Bit5_Pin);
    GPIO_ResetBits(GPIOA, DAC_Bit6_Pin | DAC_Bit7_Pin);
}



////////////////////////////////////////////
//Configure the user button on PC13
//with interrupts.  Use EXTI Line13, falling
//edge trigger.  Also, configure a button on the
//lcd shield, PA10, using exti line10, falling
//edge trigger.  it's a shared interrupt with the
//other button

void gpio_button_init(void)
{
    buttonFlag = 0;

    //init structs
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //Enable Port C clocks - AHB1 Bus
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);       //PC13 - user button
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);       //PA10 - shield button pin
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);      //clock enable for line interrupts

    //User button - pc13
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;            //input
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //normally up
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //shield button PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;            //input
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //normally up
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //interrupts - pc13 with line interrupts
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

    //interrupts - PA10 with line interrupts
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource10);

    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    //lines 10-15 share same irq
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void gpio_shieldLedToggle(void)
{
    GPIO_ToggleBits(shieldLed_GPIO_Port, shieldLed_Pin);
}

////////////////////////////////////////
//button interrupt handler for the
//user button on PC13
void gpio_button_handler(void)
{
    //test which line - PC13 - board button
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
//        GPIO_ToggleBits(GPIOA, GPIO_Pin_5);

        EXTI_ClearITPendingBit(EXTI_Line13);
    }

    if (EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
 //       GPIO_ToggleBits(GPIOA, GPIO_Pin_5);

        EXTI_ClearITPendingBit(EXTI_Line10);
    }

    buttonFlag = 1;     //polled in main
}



