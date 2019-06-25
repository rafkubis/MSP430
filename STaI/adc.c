/*
 * adc_opamp.c
 *
 *  Created on: 19 sty 2018
 *      Author: rafal
 */
#include <msp430.h>
#include "adc.h"
unsigned int samples[ADC_SAMPLES_QTY];
unsigned int samples_counter;
int busy;

void adc_initOpamp(void){
    P6DIR &= ~BIT0 & ~BIT4;
    P6OUT &= ~BIT0 & ~BIT4;
    P6SEL = BIT0 | BIT4;

    OA0CTL0 |= OAP_0 | OAPM_3;                          // +input OA0I0, slew rate - fast
    OA0CTL1 |= OAFBR_7 | OAFC_1;                        // -input Tap 7, Mode Unity Gain Buffer
    OA1CTL0 |= OAP_0 | OAPM_3;                          // +input OA1I0, slew rate - fast
    OA1CTL1 |= OAFBR_0 | OAFC_7;                        // -input 0 - OA1Out, Mode Differential
    OA2CTL0 |= OAP_3 | OAN_3 | OAADC0 | OAPM_3;         // +input OA0Tab, -input OA2Tab, output a14, slew rate - fast
    OA2CTL1 |= OAFBR_7 | OAFC_6;                        // Tab 7 , Mode Inverting
}

void adc_init(void){
    adc_initOpamp();
    ADC12CTL0 |= ADC12ON;                               // ADC12 enable
    ADC12CTL0 &= ~ENC;                                  // disable conversion
    ADC12CTL0 |= SHT0_14 | REFON;                       // 1024 cycles sampling, one trigger for sequence, ref 1.5V;
    ADC12CTL1 |= CSTARTADD_0 | CONSEQ_0 | TBSSEL_3;     // Conversion start address MEM0, Single channel conversion, Clock MCLK not divided
    ADC12MCTL0 |= SREF_1 | INCH_14 | EOS;               // reference internal, channel a14, end of sequence
    samples_counter = 0;
    ADC12IE |= 0x01;                                    // Enable interrupt for MEM0
}
void adc_startConversion(void){
    samples_counter = 0;
    ADC12CTL0 |= ENC | ADC12SC;                         // enable conversion, start sample and conversiom
    ADC12IE |= 0x01;                                    // Enable interrupt for MEM0
    busy = 1;
}
int adc_checkBusy(void){
    return busy;
}
unsigned int adc_readData(void){
    unsigned long data =0;
    int i = 0;
    for(i=0; i<ADC_SAMPLES_QTY; i++){
        data += (unsigned long)(samples[i]);
    }
    return data / ADC_SAMPLES_QTY;
}

#pragma vector=ADC12_VECTOR
__interrupt void adc_interrupt(void){
    if(samples_counter < ADC_SAMPLES_QTY){
        samples[samples_counter] = ADC12MEM0;
        if(samples_counter < (ADC_SAMPLES_QTY-1)){
            ADC12CTL0 |= ENC | ADC12SC;                         // enable conversion, start sample and conversiom
        }
        if(samples_counter == (ADC_SAMPLES_QTY-1)){
            busy = 0;
        }
    }
    samples_counter++;
}
