/*
 * clock.c
 *
 *  Created on: 19 sty 2018
 *      Author: rafal
 */
#include <msp430.h>

//                     fclk = 32 * 8 * 32.768kHz = 8388.6 kHz
void clock_8MHz(void){
    FLL_CTL0 |= DCOPLUS;                // DCO output is not divided
    SCFI0 |= FLLD_8 | FN2;              // DCO output / 8
    int i;
    for(i=0; i<6; i++){
        clock_wait(0xffff);
    }
}
void clock_wait(unsigned int us){
    volatile unsigned int i;
    for (i =us ; i > 0 ; i--);
}
