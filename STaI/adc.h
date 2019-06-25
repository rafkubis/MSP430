/*
 * adc.h
 *
 *  Created on: 19 sty 2018
 *      Author: rafal
 */

#ifndef ADC_H_
#define ADC_H_

#define ADC_OPAMP_GAIN      15
#define ADC_SAMPLES_QTY     10
void adc_initOpamp(void);
void adc_init(void);
void adc_startConversion(void);
int adc_checkBusy(void);
unsigned int adc_readData(void);


#endif /* ADC_H_ */
