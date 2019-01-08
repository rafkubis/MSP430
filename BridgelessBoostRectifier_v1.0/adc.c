/*
 * adc.c
 *
 *  Created on: 8 sty 2019
 *      Author: rafal
 */

#include <adc.h>

void adc_Initalize(void){
    /*      ADC gpio config
     *      ANALOG3 <-> P8.5 <-> A6
     *      ANALOG4 <-> P8.7 <-> A4
     */
    P8SEL0 |= BIT5 | BIT7;
    P8SEL1 |= BIT5 | BIT7;

    /*
     *      ADC config
     */
    ADC12CTL0 &= ~ADC12ENC;                     // disable conversion
    ADC12CTL0 |= ADC12MSC | ADC12ON;            // auto trigger , adc on
    ADC12CTL1 |= ADC12SSEL_3 | ADC12CONSEQ_1 | ADC12SHP;    // SMCLK, sequence
    ADC12MCTL0 |= ADC12INCH_6;
    ADC12MCTL1 |= ADC12INCH_4 | ADC12EOS;
    ADC12CTL3 |= ADC12CSTARTADD_0;              // start address

}

void adc_startConversion(void){
    ADC12CTL0 |= ADC12ENC | ADC12SC;
}



