/*
 * pwm.c
 *
 *  Created on: 21 paŸ 2018
 *      Author: rafal
 */
#include "pwm.h"
#include "msp430.h"

void pwm_Initalize(void){
       /*
	   P1DIR |= BIT6 | BIT7;
	   P1SEL0 |= BIT6 | BIT7;
	   P1SEL1 |= BIT6 | BIT7;
	   TA0CTL |= TASSEL_2 | ID_0 | MC_1;
	   TA0CCR0 = MAX_DUTY;
	   TA0CCTL1 |= OUTMOD_7;
	   TA0CCTL2 |= OUTMOD_7;
	//   TA0CCR1 = DUTY;
	//   TA0CCR2 = MAX_DUTY;
	 *
	 */
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
    TB0CCR0 = PWM_DIV;
    TB0CCTL5 |= OUTMOD_7;
    TB0CCTL6 |= OUTMOD_7;
    TB0CCTL3 |= OUTMOD_7;
    TB0CCTL2 |= OUTMOD_7;


}

