/*
 * clock.c
 *
 *  Created on: 23 lis 2018
 *      Author: rafal
 */
#include "clock.h"

    /*
     *      DCO 8MHz
     *      DCO -> SMCLK
     *      LF  -> MCLK, ACLK
     */
void clock_Initalize(void){
    PJSEL0 |= BIT4 | BIT5;      // LF enable -- 32.768kHz
    P5SEL1 &= ~(BIT4 | BIT5);   // LF enable
    CSCTL0 = 0xA500;                // CSKEY
    CSCTL1 = DCOFSEL_6;   // DCO frequency = 8MHz
    CSCTL0 = 0xA500;                // CSKEY
    CSCTL2 = SELA_0 | SELS_3 | SELM_3; // ACLK = LF, SMCLK = DCO, MCLK = SMCLK
    CSCTL0 = 0xA500;                // CSKEY
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // ACLK/1 . SMCLK/1, MCLK/1;
}


