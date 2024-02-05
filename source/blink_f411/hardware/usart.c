/*
 * uart.c
 *
 *  Created on: Jan 28, 2024
 *      Author: henry
 *
 *
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "usart.h"
#include "main.h"
#include "hardware.h"
#include "cli.h"

///////////////////////////////////////////
//usart2 receive buffers and counters
//use 2 buffers and flip-flop
static volatile uint8_t rxBuffer1[RX_BUFFER_SIZE];
static volatile uint8_t rxBuffer2[RX_BUFFER_SIZE];
static volatile uint8_t rxIndex;
static volatile uint8_t rxActiveBuffer;


////////////////////////////////////////////
//Configure uart2 on PA2 (tx) and PA3 (rx)
//with rx interrupts.
void usart2_init(void)
{
    memset((uint8_t*)rxBuffer1, 0x00, RX_BUFFER_SIZE);
    memset((uint8_t*)rxBuffer2, 0x00, RX_BUFFER_SIZE);

    rxIndex = 0;
    rxActiveBuffer = 1;

    //init structs
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //enable clocks for GPIO and USART2
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    //set up the tx and rx pins on PA2 and PA3
    //PA2 - TX, PA3 - RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //config AF for PA2 and PA3
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    //configure USART2
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    //enable the USART RX not empty interrupt
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    //configure usart2 interrupts
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //enable the usart2
    USART_Cmd(USART2, ENABLE);
}


void usart2_txByte(uint8_t data)
{
    while(!(USART2->SR & USART_SR_TC));
    USART_SendData(USART2, data);
    while(!(USART2->SR & USART_SR_TC));
}

void usart2_txData(uint8_t *data, uint8_t length)
{
    uint8_t i;

    for (i = 0 ; i < length ; i++)
        usart2_txByte(data[i]);
}

void usart2_txString(const char* data)
{
    uint8_t length = strlen(data);
    uint8_t i = 0;

    for (i = 0 ; i < length ; i++)
        usart2_txByte(data[i]);
}

///////////////////////////////////////////////
void usart2_txStringLength(uint8_t* data, uint8_t length)
{
    uint8_t i = 0;

    for (i = 0 ; i < length ; i++)
        usart2_txByte(data[i]);
}

////////////////////////////////////////
//called from the system_it.c
//Recieve interrupt handler for usart2 - USB port
//on stlink.  incoming data bytes put into active
//buffer.  once buffer is terminated with \n,
//data is sent for processing, buffer is flipped,
//counter resets, and process continues.
//
void usart2_interrupt_handler(void)
{
    uint8_t temp = 0;
    uint8_t data = 0;

    //test RXNE flag
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        data = USART2->DR;

        if (rxIndex < (RX_BUFFER_SIZE - 1))
        {
            if (rxActiveBuffer == 1)
            {
                rxBuffer1[rxIndex] = data;
                rxIndex++;

                if ((data == 0x00) || (data == '\n'))
                {
                    //end of the string
                    rxBuffer1[rxIndex] = 0x00;
                    temp = rxIndex - 1;
                    rxBuffer1[rxIndex - 1] = 0x00;

                    //flip the buffers for new data incoming to rxBuffer2
                    memset((uint8_t*)rxBuffer2, 0x00, RX_BUFFER_SIZE);
                    rxActiveBuffer = 2;
                    rxIndex = 0x00;

                    //process the command using temp and rxBuffer1
                    cli_processBuffer((uint8_t*)rxBuffer1, temp);
                }
            }

            //active buffer = 2
            else
            {
                rxBuffer2[rxIndex] = data;
                rxIndex++;

                if ((data == 0x00) || (data == '\n'))
                {
                    //end of the string
                    rxBuffer2[rxIndex] = 0x00;
                    temp = rxIndex - 1;
                    rxBuffer2[rxIndex - 1] = 0x00;

                    //flip the buffers for new data incoming to rxBuffer1
                    memset((uint8_t*)rxBuffer1, 0x00, RX_BUFFER_SIZE);
                    rxActiveBuffer = 1;
                    rxIndex = 0x00;

                    //process the command using temp and rxBuffer2
                    cli_processBuffer((uint8_t*)rxBuffer2, temp);
                }
            }
        }

        //else, buffer overrun - clear all the buffers and
        //reset to active buffer 1
        else
        {
            memset((uint8_t*)rxBuffer1, 0x00, RX_BUFFER_SIZE);
            memset((uint8_t*)rxBuffer2, 0x00, RX_BUFFER_SIZE);
            rxIndex = 0x00;
            rxActiveBuffer = 1;
        }
    }
}




