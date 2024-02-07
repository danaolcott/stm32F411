/*
 * joystick.c
 *
 *  Created on: Feb 6, 2024
 *      Author: henry
 */

#include <stdint.h>
#include <stddef.h>

#include "main.h"
#include "joystick.h"
#include "adc.h"


//////////////////////////////////////////////////
//Read value of ADC - Channel 1.
//Read the value of the ADC1->DR register directly.
//there's something not consistent about how the
//Dma is working.
//TODO: figure out why the dma overrun flag is setting
//in the adc peripheral
//
JoystickPosition_t Joystick_GetPosition(void)
{
    uint16_t value = (uint16_t)(ADC1->DR);

    if (value < JOYSTICK_LIMIT_0)
        return JOYSTICK_LEFT;
    else if ((value >= JOYSTICK_LIMIT_0) && (value < JOYSTICK_LIMIT_1))
        return JOYSTICK_PRESS;
    else if ((value >= JOYSTICK_LIMIT_1) && (value < JOYSTICK_LIMIT_2))
        return JOYSTICK_DOWN;
    else if ((value >= JOYSTICK_LIMIT_2) && (value < JOYSTICK_LIMIT_3))
        return JOYSTICK_NONE;
    else if ((value >= JOYSTICK_LIMIT_3) && (value < JOYSTICK_LIMIT_4))
        return JOYSTICK_RIGHT;
    else if (value >= JOYSTICK_LIMIT_4)
        return JOYSTICK_UP;

    return JOYSTICK_NONE;
}


