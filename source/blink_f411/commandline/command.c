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

///////////////////////////////////////////////
//static function prototype defs
//see below for function definitions
static void cmdHelp(int argc, char** argv);
static void cmdEcho(int argc, char** argv);
static void cmdADC(int argc, char** argv);

////////////////////////////////////////////
//CommandStruct commandTable

static const CommandStruct commandTable[4] =
{
    {"?",       "Print Help",   cmdHelp},
    {"echo",    "echo args and num args", cmdEcho},
    {"adc",    "Register dump of adc1", cmdADC},
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


