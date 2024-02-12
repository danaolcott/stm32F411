/*
 * timer.c
 *
 *  Created on: Feb 7, 2024
 *      Author: henry
 */
#include <stddef.h>
#include <stdint.h>
#include "main.h"
#include "timer.h"

volatile uint32_t gTimer2Counter = 0;


/////////////////////////////////////////////////
//Configure Timer2 with interrupts
//that trigger at the Timer_Speed
//Timer2 is on APB1.
//
void timer2_init(Timer_Speed hz)
{
    NVIC_InitTypeDef NVIC_InitStructure;                //for the interrupts
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;     //timers

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //timebase configure
    uint16_t PrescalerValue = (uint16_t)48 - 1;
    uint16_t reloadValue = 999;

    //set the prescale and reload counter based
    //on the timer frequency
    switch(hz)
    {
        case TIMER_SPEED_10HZ:
        {
            PrescalerValue = (uint16_t)96 - 1;
            reloadValue = 49999;
            break;
        }
        case TIMER_SPEED_100HZ:
        {
            PrescalerValue = (uint16_t)48 - 1;
            reloadValue = 9999;
            break;
        }
        case TIMER_SPEED_1KHZ:
        {
            PrescalerValue = (uint16_t)48 - 1;
            reloadValue = 999;
            break;
        }
        case TIMER_SPEED_8KHZ:
        {
            PrescalerValue = (uint16_t)6 - 1;
            reloadValue = 999;
            break;
        }

        default:
        {
            PrescalerValue = (uint16_t)48 - 1;
            reloadValue = 999;
            break;
        }
    }

    TIM_TimeBaseStructure.TIM_Period = reloadValue;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    //start the timer and enable interrupts
    //or use the timer2_start / stop functions
    TIM_Cmd(TIM2, ENABLE);

    //Configure Timer7 overflow interrupt
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    timer2_stop();
}


void timer2_start(void)
{
    TIM_Cmd(TIM2, ENABLE);
}

void timer2_stop(void)
{
    TIM_Cmd(TIM2, DISABLE);
}

void timer2_toggle(void)
{
    if (TIM2->CR1 & TIM_CR1_CEN)
        TIM_Cmd(TIM2, DISABLE);
    else
        TIM_Cmd(TIM2, ENABLE);
}

//////////////////////////////////////////
//Timer2 configured to generate interrupt
//each time it down counts to 0 - timeout
void timer2_interrupt_handler(void)
{
    //test the interrupt source
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        //clear the interrupt
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        gpio_shieldLedToggle();

        //for playing the sound
        wav_soundInterruptHandler();

        gTimer2Counter++;
    }
}





