/*
 * utility.c
 *
 *  Created on: Feb 5, 2024
 *      Author: henry
 */


#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "utility.h"


//////////////////////////////////////////////////////////////////
//Utility function for parsing a string of chars into an array
//of pointers pointing to the start of each argument and the
//number of arguments in the string.  Replaces all white space
//and tokens with 0x00 in the orginal input buffer.
//buffersize = max length of the pointer buffer  **argv
//NOTE: original input buffer "in" is modified.
void utility_parseArgs(char *in, uint8_t *pargc, char **argv, uint8_t bufferSize)
{
    int argc = 0;
    char* ptr;

    //get the first arg - pass input buffer
    ptr = strtok(in, " ,-");
    argv[argc++] = ptr;

    //subsequent args, pass NULL
    while ((ptr != NULL) && (argc < bufferSize))
    {
        ptr = strtok(NULL, " ,-");
        if(ptr != NULL)
            argv[argc++] = ptr;
    }

    *pargc = argc;
}


////////////////////////////////////////////////
//prints a 16 bit value into a buffer of maximum
//size, returns length of chars in the buffer
uint8_t utility_decimalToBuffer(uint16_t val, uint8_t* buffer, uint8_t size)
{
    uint8_t i = 0;
    char digit;
    uint8_t num = 0;
    char t;

    //clear the buffer
    memset(buffer, 0x00, size);

    //test for a zero value
    if (val == 0)
    {
        buffer[0] = '0';
        buffer[1] = 0x00;
        return 1;
    }

    while (val > 0)
    {
        digit = (char)(val % 10);
        buffer[num] = (0x30 + digit) & 0x7F;
        if (num < size)         //check for overflow
            num++;
        else
            return 0;           //error
        val = val / 10;
    }

    //reverse in place
    for (i = 0 ; i < num / 2 ; i++)
    {
        t = buffer[i];
        buffer[i] = buffer[num - i - 1];
        buffer[num - i - 1] = t;
    }

    buffer[num] = 0x00;     //null terminated

    return num;
}



//////////////////////////////////////////////////
//prints a value out to a buffer in binary format
//size = max size of the buffer.  returns the number
//of chars printed into the buffer.  inserts a space
//for every 4 bits
uint8_t utility_decimalToBinary(uint16_t val, uint8_t* buffer, uint8_t size)
{
    uint8_t i = 0;
    uint8_t num = 0;
    char t;

    //clear the buffer
    memset(buffer, 0x00, size);

    //test for a zero value
    if (val == 0)
    {
        buffer[0] = '0';
        buffer[1] = 0x00;
        return 1;
    }

    while (val > 0)
    {
        if (!(val % 2))
            buffer[num] = (0x30 + 0);
        else
            buffer[num] = (0x30 + 1);

        if (num < size)         //check for overflow
            num++;
        else
            return 0;           //error

        val = val >> 1;
    }

    //reverse in place
    for (i = 0 ; i < num / 2 ; i++)
    {
        t = buffer[i];
        buffer[i] = buffer[num - i - 1];
        buffer[num - i - 1] = t;
    }

    buffer[num] = 0x00;     //null terminated

    return num;

}




