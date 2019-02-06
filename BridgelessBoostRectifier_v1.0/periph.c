/*
 * periph.c
 *
 *  Created on: 8 sty 2019
 *      Author: rafal
 */

#include "periph.h"

    /*
     *      DCO 8MHz
     *      DCO -> SMCLK, MCLK
     *      LF  -> ACLK
     */
void clock_Initalize(void){
    PJSEL0 |= BIT4 | BIT5;      // LF enable -- 32.768kHz
    P5SEL1 &= ~(BIT4 | BIT5);   // LF enable
    CSCTL0 = 0xA500;                // CSKEY
    CSCTL1 = DCOFSEL_6;   // DCO frequency = 8MHz
    CSCTL0 = 0xA500;                // CSKEY
    CSCTL2 = SELA_0 | SELS_3 | SELM_3; // ACLK = LF, SMCLK = DCO, MCLK = DCO
    CSCTL0 = 0xA500;                // CSKEY
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // ACLK/1 . SMCLK/1, MCLK/1;
}

void adc_Initalize(void){
    /*      ADC gpio config
     *      ANALOG3 <-> P8.5 <-> A6
     *      ANALOG4 <-> P8.7 <-> A4
     */
    P8SEL0 |= BIT5 | BIT7;
    P8SEL1 |= BIT5 | BIT7;

    /*
     *      ADC config
     *      12 bit, ref 3.3V
     */
    ADC12CTL0 &= ~ADC12ENC;                     // disable conversion
    ADC12CTL0 |= ADC12MSC | ADC12ON | ADC12SHT0_10;            // auto trigger , adc on, 512 clk sampling
    ADC12CTL1 |= ADC12SSEL_3 | ADC12CONSEQ_1 | ADC12SHP;    // SMCLK, sequence
    ADC12MCTL0 |= ADC12INCH_6;                  // ch 6
    ADC12MCTL1 |= ADC12INCH_4 | ADC12EOS;       // ch 4, end of sequence
    ADC12CTL3 |= ADC12CSTARTADD_0;              // start address
    ADC12IER0 |= ADC12IE1;                      // mem1 interrupt enable

}

void adc_startConversion(void){
    ADC12CTL0 |= ADC12ENC | ADC12SC;
}

void pwm_Initalize(void){
    /*
     *      PWM configuration:
     *      TB0.5 -> P2.1 -> PWM3 -> GATE2.2
     *      TB0.6 -> P2.7 -> PWM2 -> GATE1.1
     *
     *      TB0.3 -> P3.7 -> PWM1 -> GATE2.1
     *      TB0.2 -> P3.6 -> PWM4 -> GATE1.2
     *
     *      I/O configuration
     */
    P2DIR |= BIT1 | BIT7;       // output for PWM3, PWM2
    P3DIR |= BIT6 | BIT7;       // output for PWM4, PWM1

    P2SEL0 |= BIT7;             // alternate functions
    P2SEL1 |= BIT1;             //
                                //
    P3SEL1 |= BIT6 | BIT7;      //
    /*
     *      TIMER B configuration
     */
    TB0CTL = TBSSEL__SMCLK | ID__1 | MC__UP;    // clock SMCL, clock/1, up to mode
    TB0CCR0 = 0xffff;
    TB0CCTL5 |= OUTMOD_7;
    TB0CCTL6 |= OUTMOD_7;
    TB0CCTL3 |= OUTMOD_7;
    TB0CCTL2 |= OUTMOD_7;


}

void timerA_Initalize(void){
    /*  ACLK / 1 , up to TA0CCR0 mode, interrupt enable
     *  clk = 32.768kHz
     *  20 Hz - interrupt
     */
    TA0CTL |= TASSEL__ACLK | ID__1 | TAIE;
    TA0CCR0 = 820;
}

inline void timerA_Stop(void){
    TA0CTL &= ~MC_1;        // reset MC_1 bits
}

inline void timerA_Start(void){
    TA0CTL |= MC_1;         //  up mode
}
