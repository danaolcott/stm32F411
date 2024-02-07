/*
 * adc.h
 *
 *  Created on: Feb 3, 2024
 *      Author: henry
 */

#ifndef ADC_H_
#define ADC_H_

uint16_t adc_getError(void);
void adc_init(void);
uint16_t adc_getValueFromDMA(void);
uint16_t adc_getSingleScanValue(void);
void adc_interrupt_handler(void);


#endif /* ADC_H_ */
