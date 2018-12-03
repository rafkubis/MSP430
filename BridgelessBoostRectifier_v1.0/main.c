#include <msp430.h> 
#include "clock.h"
#include "pwm.h"
#include "bridge_boost_rect.h"

/**
 * main.c
 */
BoostRect boostRect[2];

void main(void)
{
    PM5CTL0 = 0;    // unlock I/O pins
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	clock_Initalize();
	/*
	 *      IO conFiguration
	 *      P2.6 -> input pull down
	 *      P2.6 <-> P2.7
	 */
	P2DIR &= ~BIT6;
	P2REN |= BIT6;
	P2OUT &= ~BIT6;



	pwm_Initalize();
	/*
	 *      External IO interrupt config
	 *          SIGN1 -> P2.0
	 *          SIGN2 -> P3.2
	 */
	P2DIR &= ~BIT0;     //  input
	P2OUT |= BIT0;      //  pull down
	P2REN |= BIT0;      //  Pull up/down enable
	P2IES |= BIT0;      //  H to L slope
	P2IE |= BIT0;       //  interrupt enable

    P3DIR &= ~BIT2;     //  input
    P3OUT |= BIT2;      //  pull down
    P3REN |= BIT2;      //  Pull up/down enable
    P3IES |= BIT2;      //  H to L slope
    P3IE |= BIT2;       //  interrupt enable


	P1DIR |= BIT0;
	P1OUT |= BIT0;
    P9DIR |= BIT7;
    P9OUT |= BIT7;

    boostRect[0].T1_pwm_register = &TB0CCR6;
    boostRect[0].T2_pwm_register = &TB0CCR3;
    boostRect[1].T1_pwm_register = &TB0CCR2;
    boostRect[1].T2_pwm_register = &TB0CCR5;
    boostRect[0].pwm_max = PWM_DIV;
    boostRect[1].pwm_max = PWM_DIV;
    boostRect[0].pwm = 10;
    boostRect[1].pwm = 10;
//    BoostRect_PositiveSign(&boostRect[0]);
//    BoostRect_PositiveSign(&boostRect[1]);
	__enable_interrupt();
	while(1){

	}
}
/*
 *      External IO interrupt form PORT2
 *
 */
#pragma vector=PORT2_VECTOR
__interrupt void Port2_interrupt(void){
    if(P2IES & BIT0) {      // check if rising edge is selected
        P2IES ^= BIT0;      //  change edge
        P9OUT |= BIT7;
        BoostRect_PositiveSign(&boostRect[1]);
    } else{
        P2IES ^= BIT0;      //  change edge
        P9OUT &= ~BIT7;
        BoostRect_NegativeSign(&boostRect[1]);
    }
    P2IV;
}
/*
 *      External IO interrupt from PORT3
 *      BoostRect 0 sign
 */


#pragma vector=PORT3_VECTOR
__interrupt void Port3_interrupt(void){
    if(P3IES & BIT2) {      // check if rising edge is selected
        P3IES ^= BIT2;      //  change edge
        P1OUT |= BIT0;
        BoostRect_PositiveSign(&boostRect[0]);
    } else{
        P3IES ^= BIT2;      //  change edge
        P1OUT &= ~BIT0;
        BoostRect_NegativeSign(&boostRect[0]);
    }
    P3IV;

}
