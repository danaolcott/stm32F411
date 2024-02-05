/*
 * cli.c
 *
 *  Created on: Feb 5, 2024
 *      Author: henry
 *
 *  Commandline interface for usart peripheral.
 */


#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "main.h"
#include "cli.h"
#include "command.h"


///////////////////////////////////////////////////////////
void cli_processBuffer(uint8_t* data, uint8_t length)
{
    uint8_t buffer[16];
    uint8_t len;

    uint8_t numArgs = 0;
    uint8_t *argv[16];

    //echo the data back out the serial port.
    usart2_txString("RX: ");
    usart2_txStringLength(data, length);
    usart2_txString("\r\n");

    len = cli_decimalToBuffer(length, buffer, 16);

    usart2_txString("Num Chars: ");
    usart2_txStringLength(buffer, len);
    usart2_txString("\r\n");

    //parse the string into list of commands
    //note:  data buffer is the same as cmdBuffer
    //cmdBuffer will get modified with all white space
    //replaced with 0x00.  numArgs will be populated with
    //number of commands, argv is array of pointers
    //pointing to the data buffer
    cli_parseArgs((char*)data, &numArgs, (char**)argv, 16);

    //pass numArgs and argv into the command lookup
    //table and execute the command
    command_exeCommand(numArgs, (char**)argv);


}




//////////////////////////////////////////////////////////////////
//Utility function for parsing a string of chars into an array
//of pointers pointing to the start of each argument and the
//number of arguments in the string.  Replaces all white space
//and tokens with 0x00 in the orginal string
//buffersize = max length of the pointer buffer buffer
void cli_parseArgs(char *in, uint8_t *pargc, char **argv, uint8_t bufferSize)
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



/////////////////////////////////////////////////////////
//writes val into buffer of maximum "size"  returns the
//number of bytes written
//
uint8_t cli_decimalToBuffer(uint16_t val, uint8_t* buffer, uint8_t size)
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

