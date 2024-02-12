/*
 * spi.c
 *
 *  Created on: Jan 28, 2024
 *      Author: henry
 *      //spi interface for lcd
#define SPI1_CS_Pin GPIO_Pin_6
#define SPI1_CS_GPIO_Port GPIOB

#define SPI1_SCK_Pin GPIO_Pin_5
#define SPI1_MISO_Pin GPIO_Pin_6
#define SPI1_MOSI_Pin GPIO_Pin_7
#define SPI1_GPIO_Port GPIOA

NOTE:  Going forward, use PB10 as the CS Pin
for the LCD.  PB6 will be used as the cs pin for
the SD Card



 *
 */

#include <stddef.h>
#include <stdint.h>

#include "main.h"
#include "spi.h"


//////////////////////////////////////////
//Configure SPI1 on the following pins for
//controlling the display
//#define SPI1_CS_Pin GPIO_Pin_6
//#define SPI1_CS_GPIO_Port GPIOB

//#define SPI1_SCK_Pin GPIO_Pin_5
//#define SPI1_MISO_Pin GPIO_Pin_6
//#define SPI1_MOSI_Pin GPIO_Pin_7
//#define SPI1_GPIO_Port GPIOA
//
//Base speed for APB2 should be 48mhz.
//based on prescale = 8, should run the spi at 6mhz
void spi1_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef   SPI_InitStruct;

    //enable the clocks - PortA, PortB, SPI1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    //cs pin as output - PB6 - SD Card CS pin
    //This is mapped to D10 on the arduino labels
    //cs pin as output - PB10 - CS Pin for the LCD
    //this is mapped to D6 on the arduino labels
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //should be pulled up
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //PA5, PA6, PA7 as alternate function - SPI1
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //alternate function
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //map SPI1 pins to alternate function
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

    //configure the SPI1 interface - idle clock low, leading edge
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;      //8 bits
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;            //idle low?  high
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;           //leading?  trailing
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;    //handle CS pin another way

    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;     //should put this at 6mhz
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

    SPI_Init(SPI1, &SPI_InitStruct);
    SPI_Cmd(SPI1, ENABLE); // enable SPI1

    //set the cs pin high
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    GPIO_SetBits(GPIOB, GPIO_Pin_10);
}


/////////////////////////////////////////////
//sets the speed of the spi controller
//this is used to change the speed from slow
//during initialize the sdcard and after it's
//initialized can run it faster
//speed choices are 2mhz, 1mhz, and 400khz.
//need this function to change the speed of the spi
//during initialization of the sd card.
//
//disables the spi.  makes a new init struct,
//initializes it with the new struct, and reenables
//Assumes the system clock is running at 16mhz, which
//is the default for no pll HSI clock.

void spi1_setSpeedHz(SPI_Speed_t speed)
{
    SPI_InitTypeDef   SPI_InitStruct;

    SPI_Cmd(SPI1, DISABLE); // disable the SPI1

    //do we need to shut this down
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;      //8 bits
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;            //idle low?  high
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;           //leading?  trailing
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;    //handle CS pin another way
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;     //should put this at 12mhz
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

    //values based on 16mhz clock - no pll
//    switch(speed)
//    {
//        case SPI_SPEED_2MHZ:     SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;        break;
//        case SPI_SPEED_1MHZ:     SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;       break;
//        case SPI_SPEED_400KHZ:   SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;       break;
//        default:                 SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;       break;
//    }

    //values based on 48mhz clock
    switch(speed)
    {
        case SPI_SPEED_12MHZ:     SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;        break;
        case SPI_SPEED_6MHZ:     SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;        break;
        case SPI_SPEED_3MHZ:     SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;       break;
        case SPI_SPEED_400KHZ:   SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;       break;
        default:                 SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;       break;
    }

    SPI_Init(SPI1, &SPI_InitStruct);
    SPI_Cmd(SPI1, ENABLE); // enable SPI1
}

void spi1_lcd_select(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_10);
}

void spi1_lcd_deselect(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_10);
}

void spi1_sdcard_select(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
}

void spi1_sdcard_deselect(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
}

uint8_t spi1_txByte(uint8_t data)
{
    SPI1->DR = data;
    while(SPI1->SR & SPI_I2S_FLAG_BSY){}    // wait until SPI is not busy anymore
    while(!(SPI1->SR & SPI_I2S_FLAG_TXE));  // wait until transmit complete - required
    while(!(SPI1->SR & SPI_I2S_FLAG_RXNE)); // wait until receive complete - required
    while(SPI1->SR & SPI_I2S_FLAG_BSY){}    // wait until SPI is not busy anymore

    //return the received data if any
    return SPI1->DR;
}

/////////////////////////////////////////
//spi receive byte - no chip select
//sends a dummy byte 0xFF and reads the
//data register, and returns the value
uint8_t spi1_rxByte(void)
{
    SPI1->DR = 0xFF;
    while(SPI1->SR & SPI_I2S_FLAG_BSY){}    // wait until SPI is not busy anymore
    while(!(SPI1->SR & SPI_I2S_FLAG_TXE));  // wait until transmit complete - required
    while(!(SPI1->SR & SPI_I2S_FLAG_RXNE)); // wait until receive complete - required
    while(SPI1->SR & SPI_I2S_FLAG_BSY){}    // wait until SPI is not busy anymore

    //return the received data if any
    return SPI1->DR;
}


void spi1_lcd_txData(uint8_t* data, uint8_t length)
{
    uint8_t i = 0;

    spi1_lcd_select();

    for (i = 0 ; i < length ; i++)
    {
        spi1_txByte(data[i]);
    }

    spi1_lcd_deselect();
}



