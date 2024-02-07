/*
 * joystick.h
 *
 *  Created on: Feb 6, 2024
 *      Author: henry
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>
#include <stddef.h>


//////////////////////////////////////////
//Joystick limits for reading various positions.
//on ADC channel 6, labeled as AD1.  These
//set as approx. halfway points from joystick presses.
//
//NOTE:
//These limits work with 2 of the displays
//but not the two newest ones for some reason.
//the scope output shows ok, but something
//is going on with the display
//#define JOYSTICK_LIMIT_0      600
//#define JOYSTICK_LIMIT_1      1000
//#define JOYSTICK_LIMIT_2      1800
//#define JOYSTICK_LIMIT_3      2800
//#define JOYSTICK_LIMIT_4      3600


#define JOYSTICK_LIMIT_0        600
#define JOYSTICK_LIMIT_1        2000
#define JOYSTICK_LIMIT_2        3000
#define JOYSTICK_LIMIT_3        3500
#define JOYSTICK_LIMIT_4        4000

typedef enum
{
    JOYSTICK_UP,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_PRESS,
    JOYSTICK_NONE
}JoystickPosition_t;



JoystickPosition_t Joystick_GetPosition(void);



#endif /* JOYSTICK_H_ */
