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
//Function prototypes
//configure the led on PA5
//note:  PA5 will eventually change to one of the
//spi pins for the lcd.
void gpio_init(void)
{
    buttonFlag = 0;

    //init structs
//    GPIO_InitTypeDef  GPIO_InitStructure;

    //Enable PortA clocks
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);       //PA5 - green led

    //PA5 - green led
//    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

//    GPIO_ResetBits(GPIOA, GPIO_Pin_5);

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



