/*
 * wav.h
 *
 *  Created on: Feb 8, 2024
 *      Author: henry
 */

#ifndef WAV_H_
#define WAV_H_

#define HEADER_SIZE         44


typedef struct
{
    uint8_t rawHeaderBuffer[HEADER_SIZE];

    uint8_t chunkID[4];     //RIFF in ascii form
    uint32_t chunkSize;     //total file size - 8
    uint8_t format[4];      //WAVE in ascii form
    uint8_t audioFormat;   //PCM = 1, values other than 1 means some type of compression
    uint8_t numChannels;   //mono = 1, stereo = 2...etc
    uint32_t sampleRate;    //ie, 8000, 44100... etc
    uint32_t byteRate;      // = SamepleRate * numchannels * bits/sample / 8
    uint8_t blockAlign;    //number of bytes for 1 sample
    uint8_t bitsPerSample; //ie, 8 bit, 16 bit... etc
    uint8_t subChunk2ID[4]; //"data"
    uint32_t subChunk2Size; //this is the number of bytes in the data.


}WavFileHeader;

void wav_init(void);
uint32_t wav_readHeader(char* fileName, WavFileHeader *header);
int wav_playSound(char* filename);
void wav_soundInterruptHandler(void);
uint8_t wav_getSoundPlayStatus(void);
void wav_setDACOutput(uint8_t value);





#endif /* WAV_H_ */
