/*
 * command.c
 *
 *  Created on: Feb 5, 2024
 *      Author: henry
 *
 *  Commandline commands for incoming data through the
 *  USB.  Some of the commands also route data to the
 *  bluetooth.
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
static void cmdTimer3(int argc, char** argv);
static void cmdPrintDir(int argc, char** argv);
static void cmdPlaySound(int argc, char** argv);
static void cmdFile(int argc, char** argv);

////////////////////////////////////////////
//CommandStruct commandTable

static const CommandStruct commandTable[9] =
{
    {"?",       "Print Help",   cmdHelp},
    {"echo",    "echo args and num args", cmdEcho},
    {"adc",    "Register dump of adc1", cmdADC},
    {"timer2",    "Toggle Timer 2", cmdTimer2},
    {"timer3",    "Timer3 on / off, set # hz", cmdTimer3},
    {"dir",    "print sdcard dir", cmdPrintDir},
    {"play",    "play current, skip, next, print, stop, pause, resume, remove, add", cmdPlaySound},
    {"file",    "delete name - deletes file name", cmdFile},
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
//Timer2 - on / off, set # hz.
//Controlls Timer2
static void cmdTimer2(int argc, char** argv)
{
    usart2_txString("Toggle Timer\r\n");

    if (argc == 1)
    {
        usart2_txString("Timer2 on/off.... set # hz (10, 100, 1000, 8000\r\n");
    }
    else if (argc == 2)
    {
        if (!strcmp(argv[1], "on"))
            timer2_start();
        else if (!strcmp(argv[1], "off"))
            timer2_stop();
        else
            usart2_txString("Invalid or missing params (set #)\r\n");
    }
    else if (argc == 3)
    {
        //reset the timer based on arg2, values in hz
        //timer2 set 1000  - resets the timer to 1khz
        if (!strcmp(argv[1], "set"))
        {
            if (!strcmp(argv[2], "10"))
                timer2_init(TIMER_SPEED_10HZ);
            else if (!strcmp(argv[2], "100"))
                timer2_init(TIMER_SPEED_100HZ);
            else if (!strcmp(argv[2], "1000"))
                timer2_init(TIMER_SPEED_1KHZ);
            else if (!strcmp(argv[2], "8000"))
                timer2_init(TIMER_SPEED_8KHZ);
            else
            {
                usart2_txString("Invalid Timer value");
            }
        }
    }
}




////////////////////////////////////////////
//Timer3 - on / off, set # hz.
//Controlls Timer2
static void cmdTimer3(int argc, char** argv)
{
    if (argc == 1)
    {
        usart2_txString("Timer3 on/off.... set # hz (10, 100, 1000, 8000\r\n");
    }
    else if (argc == 2)
    {
        if (!strcmp(argv[1], "on"))
            timer3_start();
        else if (!strcmp(argv[1], "off"))
            timer3_stop();
        else
            usart2_txString("Invalid or missing params (set #)\r\n");
    }
    else if (argc == 3)
    {
        //reset the timer based on arg2, values in hz
        //timer3 set 1000  - resets the timer to 1khz
        if (!strcmp(argv[1], "set"))
        {
            if (!strcmp(argv[2], "10"))
                timer3_init(TIMER_SPEED_10HZ);
            else if (!strcmp(argv[2], "100"))
                timer3_init(TIMER_SPEED_100HZ);
            else if (!strcmp(argv[2], "1000"))
                timer3_init(TIMER_SPEED_1KHZ);
            else if (!strcmp(argv[2], "8000"))
                timer3_init(TIMER_SPEED_8KHZ);
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
//play stop - stops the current song
//play print - prints the songlist, prev, current, and next songs
//play song "song name" - plays the song

static void cmdPlaySound(int argc, char** argv)
{
    PlayListItem *playItem;
    uint16_t length;
    if (argc == 1)
    {
        usart2_txString("play arg1 - current, skip, stop, pause, resume, print\r\n");
    }

    else if (argc == 2)
    {
        if (!strcmp(argv[1], "current"))
        {
            usart2_txString("Play Current Song: ");
            playItem = playList_getCurrentSong();
            usart2_txString(playItem->songName);
            usart2_txString("\r\n");

            //bluetooth
            usart1_txString("Playing: ");
            usart1_txString(playItem->songName);
            usart1_txString("\r\n");

            playList_playCurrentSong();

        }

        else if (!strcmp(argv[1], "next"))
        {
            usart2_txString("Play Next Song: ");
            playItem = playList_getNextSong();
            usart2_txString(playItem->songName);
            usart2_txString("\r\n");

            //bluetooth
            usart1_txString("Playing: ");
            usart1_txString(playItem->songName);
            usart1_txString("\r\n");

            playList_playNextSong();

        }

        else if (!strcmp(argv[1], "skip"))
        {
            playItem = playList_skipSong();

            usart2_txString("Skip to the next song....\r\n");
            usart2_txString("Current Song: ");
            usart2_txString(playItem->songName);
            usart2_txString("\r\n");

        }

        else if (!strcmp(argv[1], "stop"))
        {
            usart2_txString("Stop Current Song: ");
            playItem = playList_getCurrentSong();
            usart2_txString(playItem->songName);
            usart2_txString("\r\n");

            //bluetooth
            usart1_txString("Stop Playing: ");
            usart1_txString(playItem->songName);
            usart1_txString("\r\n");

            playList_stopCurrentSong();
        }

        else if (!strcmp(argv[1], "pause"))
        {
            usart2_txString("pause the current song\r\n");

            //bluetooth
            usart1_txString("Pause: ");
            playItem = playList_getCurrentSong();
            usart1_txString(playItem->songName);
            usart1_txString("\r\n");

            playList_pauseCurrentSong();

        }

        else if (!strcmp(argv[1], "resume"))
        {
            usart2_txString("resume the current song\r\n");

            usart1_txString("Resume: ");
            playItem = playList_getCurrentSong();
            usart1_txString(playItem->songName);
            usart1_txString("\r\n");

            playList_resumeCurrentSong();

        }

        else if (!strcmp(argv[1], "print"))
        {
            usart2_txString("Print the current song list\r\n");

            //use the sdcard print buffer
            length = playList_printSongList(SD_writeBuffer, SD_WRITE_BUFFER_SIZE);
            usart2_txStringLength(SD_writeBuffer, length);

            //print out the current, next, and previous songs
            playItem = playList_getPreviousSong();
            usart2_txString("Previous Song: ");
            usart2_txString(playItem->songName);
            usart2_txString("\r\n");

            playItem = playList_getCurrentSong();
            usart2_txString("Current Song: ");
            usart2_txString(playItem->songName);
            usart2_txString("\r\n");

            playItem = playList_getNextSong();
            usart2_txString("Next Song: ");
            usart2_txString(playItem->songName);
            usart2_txString("\r\n\r\n");

        }

        else
        {
            usart2_txString("missing arg or wrong entry: ");
            usart2_txString(argv[1]);
            usart2_txString("\r\n");
        }
    }

    else if (argc == 3)
    {
        //play song "song name)
        if (!strcmp(argv[1], "song"))
        {
            if (playList_isValidSong(argv[2]))
            {
                usart2_txString("Playing Song: ");
                usart2_txString(argv[2]);
                usart2_txString("\r\n");

                wav_playSound(argv[2]);
            }
            else
            {
                usart2_txString("Invalid Title (case-sensitive): ");
                usart2_txString(argv[2]);
                usart2_txString("\r\n");
            }
        }

        //remove item from playlist - play remove "songname.wav"
        else if (!strcmp(argv[1], "remove"))
        {
            if (playList_isValidSong(argv[2]))
            {
                usart2_txString("remove song from list: ");
                usart2_txString(argv[2]);
                usart2_txString("\r\n");

                //check if the name is located on 1 of the
                //3 song pointers, prev, current, next
                if (!strcmp(playList_getPreviousSong()->songName, argv[2]))
                    usart2_txString("Song name is previous song, index the pointers before removing\r\n");
                else if (!strcmp(playList_getCurrentSong()->songName, argv[2]))
                    usart2_txString("Song name is current song, index the pointers before removing\r\n");
                else if (!strcmp(playList_getNextSong()->songName, argv[2]))
                    usart2_txString("Song name is next song, index the pointers before removing\r\n");

                else
                {
                    playList_RemoveSong(argv[2]);
                }
            }

            else
            {
                usart2_txString("Invalid Title (case-sensitive): ");
                usart2_txString(argv[2]);
                usart2_txString("\r\n");
            }
        }

        else
        {
            usart2_txString("wrong or missing args, try again\r\n");
        }
    }
}



/////////////////////////////////////////////
//Simple file commands
//delete for now
static void cmdFile(int argc, char** argv)
{
    FRESULT res;

    if (argc == 3)
    {
        if (!strcmp(argv[1], "delete"))
        {
            usart2_txString("delete file: ");
            usart2_txString(argv[2]);
            usart2_txString("\r\n");

            //delete the file and rebuild the directory
            //returns the size of the new directory file
            //less than 0 means an error
            res = SD_FileDelete(argv[2]);

            if (res > 0)
            {
                usart2_txString("File delete success\r\n");
                usart2_txString("Rebuilding playlist from updated directory\r\n");
                playList_Init();
                SD_BuildDirectory("DIR.TXT");
            }

            else
                usart2_txString("Error deleting the file\r\n");
        }
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



