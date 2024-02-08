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
    TIMER_SPEED_11KHZ
}Timer_Speed;


/////////////////////////////////////////////
//Timer2 configuration, start, stop, interrupt
void timer2_init(Timer_Speed hz);
void timer2_start(void);
void timer2_stop(void);
void timer2_toggle(void);
void timer2_interrupt_handler(void);



#endif /* TIMER_H_ */
