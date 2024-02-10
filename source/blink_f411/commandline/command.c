/*
 * command.c
 *
 *  Created on: Feb 5, 2024
 *      Author: henry
 */



#include <string.h>
#include <stdio.h>

#include "command.h"
#include "usart.h"
#include "cli.h"
#include "sdcard_driver.h"

///////////////////////////////////////////////
//static function prototype defs
//see below for function definitions
static void cmdHelp(int argc, char** argv);
static void cmdEcho(int argc, char** argv);
static void cmdADC(int argc, char** argv);
static void cmdTimer2(int argc, char** argv);
static void cmdPrintDir(int argc, char** argv);
static void cmdPlaySound(int argc, char** argv);

////////////////////////////////////////////
//CommandStruct commandTable

static const CommandStruct commandTable[7] =
{
    {"?",       "Print Help",   cmdHelp},
    {"echo",    "echo args and num args", cmdEcho},
    {"adc",    "Register dump of adc1", cmdADC},
    {"timer2",    "Toggle Timer 2", cmdTimer2},
    {"dir",    "print sdcard dir", cmdPrintDir},
    {"play",    "play sound (filename)", cmdPlaySound},
    {NULL, NULL, NULL},
};



//////////////////////////////////////
//Print list of commands
void cmdHelp(int argc, char** argv)
{
    usart2_txString("Help Function\r\n");
    command_printHelp();
}


////////////////////////////////////////
//Echos the input string as list and
//number of arguments
void cmdEcho(int argc, char** argv)
{
    uint8_t buffer[16];
    uint8_t length;
    uint8_t i;

    length = cli_decimalToBuffer(argc, buffer, 16);

    usart2_txString("Echo the input string as list of commands\r\n\r\n");
    usart2_txString("Number of Args: ");
    usart2_txStringLength(buffer, length);
    usart2_txString("\r\n\r\nCommands:\r\n");

    for (i = 0 ; i < argc ; i++)
    {
        usart2_txString((char*)argv[i]);
        usart2_txString("\r\n");
    }

    usart2_txString("\r\n\r\n");
}



static void cmdADC(int argc, char** argv)
{
    uint8_t buffer[64];
    uint8_t length;

    usart2_txString("Register Dump of ADC1:\r\n");

    length = snprintf((char*)buffer, 32, "ADC1-DR: 0x%04x\r\n", (uint16_t)ADC1->DR);
    usart2_txStringLength(buffer, length);
    length = snprintf((char*)buffer, 32, "ADC1-SR: 0x%04x\r\n", (uint16_t)ADC1->SR);
    usart2_txStringLength(buffer, length);
    length = snprintf((char*)buffer, 32, "ADC1-CR1: 0x%04x\r\n", (uint16_t)ADC1->CR1);
    usart2_txStringLength(buffer, length);
    length = snprintf((char*)buffer, 32, "ADC1-CR2: 0x%04x\r\n", (uint16_t)ADC1->CR2);
    usart2_txStringLength(buffer, length);

}



////////////////////////////////////////////
//Toggle Timer 2 on and off.
static void cmdTimer2(int argc, char** argv)
{
    usart2_txString("Toggle Timer\r\n");

    if (argc == 1)
    {
        timer2_toggle();
    }
    else if (argc == 2)
    {
        if (!strcmp(argv[1], "on"))
            timer2_start();
        else
            timer2_stop();
    }
    else if (argc == 3)
    {
        //reset the timer based on arg2, values in khz
        //timer2 set 11  - resets the timer to 11khz
        if (!strcmp(argv[1], "set"))
        {
            if (!strcmp(argv[2], "1"))
                timer2_init(TIMER_SPEED_1KHZ);
            else if (!strcmp(argv[2], "8"))
                timer2_init(TIMER_SPEED_8KHZ);
            else if (!strcmp(argv[2], "11"))
                timer2_init(TIMER_SPEED_11KHZ);
            else if (!strcmp(argv[2], "22"))
                timer2_init(TIMER_SPEED_22KHZ);
            else if (!strcmp(argv[2], "44"))
                timer2_init(TIMER_SPEED_44KHZ);
            else
            {
                usart2_txString("Invalid Timer value");
            }
        }
    }
}


////////////////////////////////////////////////
//Prints the directory of the sd card
//use the sd card write buffer
//SD_writeBuffer
static void cmdPrintDir(int argc, char** argv)
{
    uint16_t length = 0;
    memset(SD_writeBuffer, 0x00, SD_WRITE_BUFFER_SIZE);

    length = SD_PrintFileToBuffer("DIR.TXT", SD_writeBuffer, SD_WRITE_BUFFER_SIZE);

    usart2_txString("Contents of the sdcard:r\n");
    usart2_txStringLength(SD_writeBuffer, length);
    usart2_txString("r\n");

}

/////////////////////////////////////////////////
//Plays a sound file from the sd card
//arg1 - file name
//assumes the file is valid
static void cmdPlaySound(int argc, char** argv)
{
    uint8_t length;
    if (argc == 2)
    {
        usart2_txString("play sound: ");
        usart2_txString(argv[1]);
        usart2_txString("\r\n");

        wav_playSound(argv[1]);
    }
}





/////////////////////////////////////////////
//Command_ExeCommand
//Takes parsed arguments, searches for the
//matching command string, runs the corresponding
//function.
//returns index of the table element, -1 if
//no match
int command_exeCommand(int argc, char** argv)
{
    int i = 0x00;

    while (commandTable[i].cmdString != NULL)
    {
        if (!strcmp(argv[0], commandTable[i].cmdString))
        {
            //run the function
            commandTable[i].cmdFunction(argc, argv);
            return i;
        }

        i++;
    }

    return -1;

}


//////////////////////////////////
//Prints a listing of all command
//match string and description
void command_printHelp(void)
{
    uint8_t i = 0x00;

    usart2_txString("Help Menu (? to print menu)\r\n\r\n");

    while (commandTable[i].cmdString != NULL)
    {
        usart2_txString((const char*)commandTable[i].cmdString);
        usart2_txString("\t\t");
        usart2_txString((const char*)commandTable[i].menuString);
        usart2_txString("\r\n");

        i++;
    }

}



