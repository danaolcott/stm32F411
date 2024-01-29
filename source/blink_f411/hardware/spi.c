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

void spi1_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef   SPI_InitStruct;

    //enable the clocks - PortA, PortB, SPI1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    //cs pin as output - PB6
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //configure for output
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //push pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;       //max is 45mhz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //no pull
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
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;            //idle low
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;           //leading
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;    //handle CS pin another way

    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

    SPI_Init(SPI1, &SPI_InitStruct);
    SPI_Cmd(SPI1, ENABLE); // enable SPI1

    //set the cs pin high
    GPIO_SetBits(GPIOB, GPIO_Pin_6);

}

void spi1_select(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
}

void spi1_deselect(void)
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

void spi1_txData(uint8_t* data, uint8_t length)
{
    uint8_t i = 0;

    spi1_select();

    for (i = 0 ; i < length ; i++)
    {
        spi1_txByte(data[i]);
    }

    spi1_deselect();
}



