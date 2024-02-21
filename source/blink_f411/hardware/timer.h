/*
 * timer.h
 *
 *  Created on: Feb 7, 2024
 *      Author: henry
 */

#ifndef TIMER_H_
#define TIMER_H_

typedef enum
{
    TIMER_SPEED_10HZ,
    TIMER_SPEED_100HZ,
    TIMER_SPEED_1KHZ,
    TIMER_SPEED_8KHZ
}Timer_Speed;

extern volatile uint32_t gTimer2Counter;

/////////////////////////////////////////////
//Timer2 configuration, start, stop, interrupt
//used for output audio data
void timer2_init(Timer_Speed hz);
void timer2_start(void);
void timer2_stop(void);
void timer2_interrupt_handler(void);


///////////////////////////////////////////////
//used for polling the adc joystick
void timer3_init(Timer_Speed hz);
void timer3_start(void);
void timer3_stop(void);
void timer3_interrupt_handler(void);




#endif /* TIMER_H_ */
