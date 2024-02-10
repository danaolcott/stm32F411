/*
 * wav.c
 *
 *  Created on: Feb 8, 2024
 *      Author: henry
 *
 *  Wav file player that reads the contents of an sdcard
 *  and outputs the data to the dac.  All wav files should be
 *  converted to low definition, 8 bit mono, 8khz.
 *
 *  For this case, the dac is a 5 bit dac made with some resistors.
 *  The volume it pretty low for now, need to add an amplifier
 *
 *  See this website for converting:
 *  https://g711.org/
 */


#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "main.h"
#include "timer.h"
#include "diskio.h"             //defines - CMD0
#include "ff.h"
#include "sdcard_driver.h"
#include "wav.h"


/////////////////////////////////////////////
//Globals
volatile static WavFileHeader gWavHeader;
volatile static uint32_t gWavDataCounter = 0x00;
volatile static uint32_t gWavBytesToRead = 0x00;
volatile static uint8_t gWavSoundIsPlaying = 0x00;
volatile FIL wavFile;


////////////////////////////////////////////////
//
void wav_init(void)
{
    gWavDataCounter = 0x00;
    gWavBytesToRead = 0x00;
    gWavSoundIsPlaying = 0x00;


}


/////////////////////////////////////////////////
//loads a WavFileHeader structure with the contents
//of the wav file header. returns the length of the databytes
uint32_t wav_readHeader(char* fileName, WavFileHeader *header)
{

    //clear the header struct buffers
    memset(header->rawHeaderBuffer, 0x00, HEADER_SIZE);

    //load the rawHeaderBuffer with info from the wav file

    int result = SD_PrintFileToBuffer(fileName, header->rawHeaderBuffer, HEADER_SIZE);

    //compute all the parameters in the header struct
    memcpy(header->chunkID, header->rawHeaderBuffer, 4);
    memcpy(header->format, header->rawHeaderBuffer + 8, 4);
    memcpy(header->subChunk2ID, header->rawHeaderBuffer + 36, 4);

    header->chunkSize = (uint32_t)(header->rawHeaderBuffer[4]);
    header->chunkSize |= (((uint32_t)(header->rawHeaderBuffer[5])) << 8);
    header->chunkSize |= (((uint32_t)(header->rawHeaderBuffer[6])) << 16);
    header->chunkSize |= (((uint32_t)(header->rawHeaderBuffer[7])) << 24);

    header->audioFormat = header->rawHeaderBuffer[20];
    header->numChannels = header->rawHeaderBuffer[22];

    //4 bytes - lsb first
    header->sampleRate = (uint32_t)(header->rawHeaderBuffer[24]);
    header->sampleRate |= (((uint32_t)(header->rawHeaderBuffer[25])) << 8);
    header->sampleRate |= (((uint32_t)(header->rawHeaderBuffer[26])) << 16);
    header->sampleRate |= (((uint32_t)(header->rawHeaderBuffer[27])) << 24);

    header->byteRate = (uint32_t)(header->rawHeaderBuffer[28]);
    header->byteRate |= (((uint32_t)(header->rawHeaderBuffer[29])) << 8);
    header->byteRate |= (((uint32_t)(header->rawHeaderBuffer[30])) << 16);
    header->byteRate |= (((uint32_t)(header->rawHeaderBuffer[31])) << 24);

    header->blockAlign = header->rawHeaderBuffer[32];
    header->bitsPerSample = header->rawHeaderBuffer[34];

    header->subChunk2Size = (uint32_t)(header->rawHeaderBuffer[40]);
    header->subChunk2Size |= (((uint32_t)(header->rawHeaderBuffer[41])) << 8);
    header->subChunk2Size |= (((uint32_t)(header->rawHeaderBuffer[42])) << 16);
    header->subChunk2Size |= (((uint32_t)(header->rawHeaderBuffer[43])) << 24);

    return (header->chunkSize - 44);
}


//////////////////////////////////////////////////////
//play sound from file on sd card.  it's assumed
//the file is type .wav, 8 bit sound, mono,
//1 byte per sample.  playing at the rate of the timer
//
int wav_playSound(char* filename)
{
    FRESULT res;

    //open a file to get the header information
    gWavBytesToRead = wav_readHeader(filename, (WavFileHeader*)&gWavHeader);
    gWavDataCounter = 0x00;

    gWavSoundIsPlaying = 1;     //0x112846 =   1124422

    //reset the counters for comparing the speed of the interrupt
    gSysTickCounter = 0;
    gTimer2Counter = 0;

    //open the file....
    res = f_open((FIL*)&wavFile, filename, FA_READ);

    if (res != FR_OK)
    {
        f_close((FIL*)&wavFile);
        SD_ErrorHandler(res);
        return (-1*((int)res));
    }

    //start the timer
    timer2_start();

    return 0;
}


////////////////////////////////////////////////
//Sound interrupt handler function.
//Put this function call in a timer interrupt
//handler that timesout at the sampling rate of the
//sound file.

void wav_soundInterruptHandler(void)
{
    FRESULT res;
    volatile uint8_t soundData;
    unsigned int bytesRead;

    if (gWavSoundIsPlaying == 1)
    {
        res = f_read((FIL*)&wavFile, (uint8_t*)&soundData, 1, &bytesRead);

        //see if we can access the file data pointer here and
        //just pass it to the set dac function

        //write the data to the DAC....
        wav_setDACOutput(soundData);

        gWavBytesToRead-=bytesRead;
        gWavDataCounter+=bytesRead;

        if (gWavBytesToRead <= 8)
        {
            //stop the timer... set a flag to be polled in main
            gWavSoundIsPlaying = 0;
            res = f_close((FIL*)&wavFile);

        }
    }

}


//////////////////////////////////////////
//polled in the main loop.  when it
//goes to 0, then stop the timer
uint8_t wav_getSoundPlayStatus(void)
{
    return gWavSoundIsPlaying;
}


//////////////////////////////////////////
//Writes "value" to the 5bit dac made from
//the following resistor values:
//Dump sound data to DAC bits
//DAC_Bit0 - DAC_Bit4, located on the following
//pins (should be all in the same row on the
//Nucleo board:
//PB13, PB14, PB15, PB1, PB2
//capture the top 5 bits in value
void wav_setDACOutput(uint8_t value)
{
    //read the value of port B and mask all the bits from the dac
    //build the dac value
    //add it to the masked value
    //write to the port
//#define DAC_Bit0_Pin GPIO_Pin_13
//#define DAC_Bit1_Pin GPIO_Pin_14
//#define DAC_Bit2_Pin GPIO_Pin_15
//#define DAC_Bit3_Pin GPIO_Pin_1
//#define DAC_Bit4_Pin GPIO_Pin_2

    //mask - clear the bits associated with the dac
    //  0001 1111 1111 1001 = 0x1FF9
    //uint16_t DAC_MASK = 0x1FF9;

//    uint16_t temp = GPIO_ReadOutputData(GPIOB);
 //   temp = temp & DAC_MASK;


    GPIO_WriteBit(DAC_Bit4_GPIO_Port, DAC_Bit4_Pin, ((value >> 7) & 0x01));
    GPIO_WriteBit(DAC_Bit3_GPIO_Port, DAC_Bit3_Pin, ((value >> 6) & 0x01));
    GPIO_WriteBit(DAC_Bit2_GPIO_Port, DAC_Bit2_Pin, ((value >> 5) & 0x01));
    GPIO_WriteBit(DAC_Bit1_GPIO_Port, DAC_Bit1_Pin, ((value >> 4) & 0x01));
    GPIO_WriteBit(DAC_Bit0_GPIO_Port, DAC_Bit0_Pin, ((value >> 3) & 0x01));


}




