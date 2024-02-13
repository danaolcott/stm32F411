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
#include "diskio.h"
#include "ff.h"
#include "sdcard_driver.h"
#include "wav.h"


/////////////////////////////////////////////
//Globals
volatile static WavFileHeader gWavHeader;
volatile static uint32_t gWavDataCounter = 0x00;
volatile static uint32_t gWavBytesToRead = 0x00;
volatile static uint8_t gWavSoundIsPlaying = 0x00;

volatile static uint8_t gWavBuffer[WAV_BUFFER_SIZE];
volatile static uint16_t gWavNumBuffersRemaining = 0;
volatile static uint16_t gWavNumBytesInBufferRemaining = 0;

volatile FIL wavFile;


////////////////////////////////////////////////
//
void wav_init(void)
{
    gWavDataCounter = 0x00;
    gWavBytesToRead = 0x00;
    gWavSoundIsPlaying = 0x00;

    memset((uint8_t*)gWavBuffer, 0x00, WAV_BUFFER_SIZE);
    gWavNumBuffersRemaining = 0;
    gWavNumBytesInBufferRemaining = 0;
}


/////////////////////////////////////////////////
//loads a WavFileHeader structure with the contents
//of the wav file header. returns the size of the file.
//Note:  The header is 44 bytes.  The data subchunk holds
//a value for how many data bytes there are.  I didn't use
//this value because sometimes after converting a file from
//mp3 to wav, etc, that value was lost.  Therefore, when
//when determining how many bytes to read, i just set it
//to the file size - 44.
uint32_t wav_readHeader(char* fileName, WavFileHeader *header)
{
    //clear the header struct buffers
    memset(header->rawHeaderBuffer, 0x00, WAV_HEADER_SIZE);

    //load the rawHeaderBuffer with info from the wav file
    int result = SD_PrintFileToBuffer(fileName, header->rawHeaderBuffer, WAV_HEADER_SIZE);

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
//Play sound from file on sdcard.  The file should be
//type .wav, 8 bit mono, sampled at 8000hz.  Timer2
//should be configured to timeout at 8khz with interrupts.
//
int wav_playSound(char* filename)
{
    FRESULT res;
    unsigned int bytesRead;

    //open a file to get the header information
    gWavBytesToRead = wav_readHeader(filename, (WavFileHeader*)&gWavHeader);

    gWavDataCounter = 0x00;     //reset the data counter

    //compute number of buffers to read - discard the remainder
    //to keep the math easy
    gWavNumBuffersRemaining = (gWavBytesToRead / (uint16_t)WAV_BUFFER_SIZE);

    memset((uint8_t*)gWavBuffer, 0x00, WAV_BUFFER_SIZE);  //clear the wav buffer

    gWavNumBytesInBufferRemaining = WAV_BUFFER_SIZE;    //reset - down counter

    gWavSoundIsPlaying = 1;     //see main loop, prevents the LCD from writing

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

    //read the first buffer
    res = f_read((FIL*)&wavFile, (uint8_t*)gWavBuffer, WAV_BUFFER_SIZE, &bytesRead);

    gWavNumBuffersRemaining--;                          //counts down
    gWavBytesToRead -= WAV_BUFFER_SIZE;                 //dec each time the SDcard is read
    gWavNumBytesInBufferRemaining = WAV_BUFFER_SIZE;    //reset the buffer counter

    //start the timer
    timer2_start();

    return 0;
}


////////////////////////////////////////////////
//Sound interrupt handler function.
//Put this function call in a timer interrupt
//handler that times out at the sampling rate of the
//sound file.

void wav_soundInterruptHandler(void)
{
    FRESULT res;
    unsigned int bytesRead;

    if (gWavSoundIsPlaying == 1)
    {
        if (gWavBytesToRead > 2)                    //extra just in case
        {
            if (gWavNumBytesInBufferRemaining > 0)
            {
                wav_setDACOutput(gWavBuffer[WAV_BUFFER_SIZE - gWavNumBytesInBufferRemaining]);
                gWavDataCounter++;
                gWavNumBytesInBufferRemaining--;
            }
            else
            {
                if (gWavNumBuffersRemaining > 0)
                {
                    //load a new buffer full of data from the SDCARD
                    //and decrement the number of buffers remaining
                    res = f_read((FIL*)&wavFile, (uint8_t*)gWavBuffer, WAV_BUFFER_SIZE, &bytesRead);
                    gWavBytesToRead-=bytesRead;         //bytes read from sdcard
                    gWavNumBuffersRemaining--;                      //dec number buffers to read
                    gWavNumBytesInBufferRemaining = WAV_BUFFER_SIZE;        //reset the buffer counter
                }
                else
                {
                    //its the last buffer and we're out of data to read
                    gWavSoundIsPlaying = 0;
                    res = f_close((FIL*)&wavFile);
                    timer2_stop();
                }
            }
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


////////////////////////////////////////////
//Writes "value" to the 5bit dac made from
//5 resisters connected to the following pins:
//PB13, PB14, PB15, PB1, PB2
//capture the top 5 bits in value
void wav_setDACOutput(uint8_t value)
{
    GPIO_WriteBit(DAC_Bit6_GPIO_Port, DAC_Bit6_Pin, ((value >> 7) & 0x01));
    GPIO_WriteBit(DAC_Bit5_GPIO_Port, DAC_Bit5_Pin, ((value >> 6) & 0x01));
    GPIO_WriteBit(DAC_Bit4_GPIO_Port, DAC_Bit4_Pin, ((value >> 5) & 0x01));
    GPIO_WriteBit(DAC_Bit3_GPIO_Port, DAC_Bit3_Pin, ((value >> 4) & 0x01));
    GPIO_WriteBit(DAC_Bit2_GPIO_Port, DAC_Bit2_Pin, ((value >> 3) & 0x01));
    GPIO_WriteBit(DAC_Bit1_GPIO_Port, DAC_Bit1_Pin, ((value >> 2) & 0x01));
    GPIO_WriteBit(DAC_Bit0_GPIO_Port, DAC_Bit0_Pin, ((value >> 1) & 0x01));
}




