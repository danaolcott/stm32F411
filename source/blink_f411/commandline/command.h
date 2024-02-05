/*
 * command.h
 *
 *  Created on: Feb 5, 2024
 *      Author: henry
 */

#ifndef COMMAND_H_
#define COMMAND_H_


typedef struct
{
    char const *cmdString;           //match string
    char const *menuString;          //menu string
    void (*cmdFunction) (int argc, char** argv);     //function to run
}CommandStruct;


int command_exeCommand(int argc, char** argv);
void command_printHelp(void);



#endif /* COMMAND_H_ */
