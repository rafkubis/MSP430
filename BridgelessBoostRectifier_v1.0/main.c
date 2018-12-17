#include <msp430.h> 
#include <string.h>
#include <stdlib.h>
#include "clock.h"
#include "pwm.h"
#include "usart.h"
#include "bridge_boost_rect.h"

/**
 * main.c
 */
int itoa(int value, char *sp, int radix);
unsigned int decodeData(char *data, unsigned int *pwm, unsigned int *pwm_max);

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
//	P2DIR &= ~BIT6;
//	P2REN |= BIT6;
//	P2OUT &= ~BIT6;

	usart_init();



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

    boostRect[0].T1_pwm_register = &TB0CCR6;        // 2
    boostRect[0].T2_pwm_register = &TB0CCR3;
    boostRect[1].T1_pwm_register = &TB0CCR2;        // 1
    boostRect[1].T2_pwm_register = &TB0CCR5;
    boostRect[0].pwm_register = &TB0CCR0;
    boostRect[1].pwm_register = &TB0CCR0;
    boostRect[0].pwm_max = PWM_DIV;
    boostRect[1].pwm_max = PWM_DIV;
    boostRect[0].pwm = 10;
    boostRect[1].pwm = 10;

//    BoostRect_ChangePwmPrams(boostRect[0], PWM_DIV)
//    BoostRect_PositiveSign(&boostRect[0]);
//    BoostRect_PositiveSign(&boostRect[1]);
	__enable_interrupt();
    volatile char data_buffer[128];
    volatile unsigned int pwm, pwm_max, boost_num;
	while(1){
	    if(usart_rx_get_line(data_buffer) == 1){
	        boost_num = decodeData(data_buffer, &pwm, &pwm_max);
	        BoostRect_ChangePwmParams(&boostRect[boost_num-1], pwm, pwm_max);
	    }



	}
}
/*
 *      BOOST 2
 *      External IO interrupt form PORT2
 *
 */
#pragma vector=PORT2_VECTOR
__interrupt void Port2_interrupt(void){
    if(P2IES & BIT0) {      // check if falling edge is selected
        P2IES &= ~BIT0;      //  change edge
        P9OUT &= ~BIT7;
        BoostRect_NegativeSign(&boostRect[1]);
    } else{
        P2IES |= BIT0;      //  change edge
        P9OUT |= BIT7;

        BoostRect_PositiveSign(&boostRect[1]);
    }
    P2IV;
}




/*
 *      BOOST 1
 *      External IO interrupt from PORT3
 *      BoostRect 0 sign
 */


#pragma vector=PORT3_VECTOR
__interrupt void Port3_interrupt(void){
    if(P3IES & BIT2) {      // check if falling edge is selected
        P3IES &= ~BIT2;      //  change edge
        P1OUT &= ~BIT0;
        BoostRect_NegativeSign(&boostRect[0]);
    } else {
        P3IES |= BIT2;      //  change edge
        P1OUT |= BIT0;
        BoostRect_PositiveSign(&boostRect[0]);
    }
    P3IV;

}

#pragma vector=USCI_A1_VECTOR
__interrupt void USART1_interrupt(void){
   if(UCA1IFG & UCTXIFG){
       usart_tx_interrupt_function();
   }
   if(UCA1IFG & UCRXIFG){
       usart_rx_interrupt_function();
   }

}


unsigned int decodeData(char *data, unsigned int *pwm, unsigned int *pwm_max){
    unsigned int boostNumber = 0;
    char *string_begin = NULL;
    char *string_end = NULL;
    char tmp_buffer[32];
    unsigned int tmp_pwm, tmp_pwm_max;
    if(data == NULL){
        return 0;
    } else{
        string_begin =  strstr(data, "BOOST");
        string_end = strstr(data, "_");
        if(string_begin != NULL && string_end != NULL){
//            string_end--;
            string_begin += strlen("BOOST");
            if((string_end - string_begin) < 3){
                memcpy(tmp_buffer, string_begin, string_end - string_begin);
                tmp_buffer[string_end - string_begin] = '\0';
                boostNumber = atoi(tmp_buffer);
                string_begin = ++string_end;
                string_end = strstr(string_begin, "_");
                if(string_end == NULL){
                    return NULL;
                }
  //              string_end--;
                memcpy(tmp_buffer, string_begin, string_end - string_begin);
                tmp_buffer[string_end - string_begin] = '\0';
                tmp_pwm = atoi(tmp_buffer);
                string_begin = ++string_end;
                string_end = strstr(string_begin, "_");
                if(string_end == NULL){
                    return NULL;
                }
 //               string_end--;
                memcpy(tmp_buffer, string_begin, string_end - string_begin);
                tmp_buffer[string_end - string_begin] = '\0';
                tmp_pwm_max = atoi(tmp_buffer);
            }
        } else {
            return NULL;
        }
    }
    *pwm = tmp_pwm;
    *pwm_max = tmp_pwm_max;
    return boostNumber;
}

// Yet, another good itoa implementation
// returns: the length of the number string
int itoa(int value, char *sp, int radix)
{
    char tmp[16];// be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;

    int sign = (radix == 10 && value < 0);
    if (sign)
        v = -value;
    else
        v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    int len = tp - tmp;

    if (sign)
    {
        *sp++ = '-';
        len++;
    }

    while (tp > tmp)
        *sp++ = *--tp;

    *sp++ = '\0';
    return len;
}
