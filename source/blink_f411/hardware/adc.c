/*
 * adc.c
 *
 *  Created on: Feb 3, 2024
 *      Author: henry
 */

#include <stddef.h>
#include <stdint.h>
#include "main.h"
#include "adc.h"


//check this for the stm32f411 - adc DR address
//from datasheet 0x4001 2000 - 0x4001 23FF
//Address offset: 0x4C
//Reset value: 0x0000 000

//this was pulled from an f072 example so address might not be correct
//#define ADC1_DR_Address    0x40012440        //map 0x40012400 + offset 0x40


//from reading the registers during debug
#define ADC1_DR_Address    0x4001204C        //value in the SFR
volatile uint32_t RawADCDataValues[2];           //single channel


///////////////////////////////////////////////
//adc_dummy delay - used to test clocks starting up
void adc_dummy_delay(volatile uint16_t time)
{
    volatile uint16_t temp = time;
    while (temp > 0)
        temp--;
}

/////////////////////////////////////////
//Configure ADC on PA1 for use with the
//joystick.  Route the ADC output through
//DMA channel for continuous readings.
//DMA2 - stream0
//ADC1 - ch0 and ch1
//I think it's ch1 that i need...
//Configure PA1 as AF, input
//
void adc_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);   //port A clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);    // ADC Clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);    //dma2 clock

    /////////////////////////////////////////////
    //configure the DMA
    //DMA2 stream 0 was taken off the cube tool startup code for the same board
    //but it also adds the interrupt for dma, maybe i need to add that here.
    //
    DMA_DeInit(DMA2_Stream0);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&RawADCDataValues[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 2;                               //number of data units - ie, 1/2 word
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;                //only 1 location to write to
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;

    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);


    //configure PA1 as analog input for adc1 ch1
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    //initialize adc common init structure
    ADC_CommonStructInit(&ADC_CommonInitStructure);
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    //init the adc structure
    ADC_StructInit(&ADC_InitStructure);
    ADC_DeInit();
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_480Cycles);

    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_SoftwareStartConv(ADC1);
}



/////////////////////////////////////////
uint16_t adc_getValueFromDMA(void)
{
    return RawADCDataValues[0];
}


///////////////////////////////////////////
uint16_t adc_getSingleScanValue(void)
{
    uint16_t result;
    ADC_SoftwareStartConv(ADC1);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    result = ADC_GetConversionValue(ADC1);

    return result;
}
