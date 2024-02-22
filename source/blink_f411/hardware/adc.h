/*
 * adc.h
 *
 *  Created on: Feb 3, 2024
 *      Author: henry
 */

#ifndef ADC_H_
#define ADC_H_

void adc_init(void);
uint16_t adc_getSingleScanValue(void);
uint16_t adc_getValueFromTimerUpdate(void);


#endif /* ADC_H_ */
