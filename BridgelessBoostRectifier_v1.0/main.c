/*
 * main.c
 *
 *  Created on: 8 sty 2019
 *      Author: rafal
 */

#include <msp430.h> 
#include <string.h>
#include <stdlib.h>
#include "periph.h"
#include "usart.h"
#include "bridge_boost.h"


#define BOOST_KP   4.5     *256
#define BOOST_KI   1.5     *256



unsigned int decodeData(char *data, unsigned int *param0, unsigned int *param1);

BoostRect boostRect[2];

volatile unsigned int adc_new_data;
volatile unsigned int regulator;

void main(void)
{
    PM5CTL0 = 0;    // unlock I/O pins
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	clock_Initalize();


	usart_init();

	adc_new_data = 0;


	adc_Initalize();
	timerA_Initalize();
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


    /*
     *      Bridgeless Boost Rectifier Structure config
     *      pwm = 0 %
     *      pwm frequency = 50 kHz
     */
    boostRect[0].T1_pwm_register = &TB0CCR6;        // 2
    boostRect[0].T2_pwm_register = &TB0CCR3;
    boostRect[1].T1_pwm_register = &TB0CCR2;        // 1
    boostRect[1].T2_pwm_register = &TB0CCR5;
    boostRect[0].pwm_register = &TB0CCR0;
    boostRect[1].pwm_register = &TB0CCR0;
    *(boostRect[0].pwm_register) = BOOSTRECT_PWM_MAX;
    boostRect[0].pwm_freq_value = BOOSTRECT_PWM_MAX;
    boostRect[1].pwm_freq_value = BOOSTRECT_PWM_MAX;
    boostRect[0].pwm = 0;
    boostRect[1].pwm = 0;

    boostRect[0].regulator_Kp = BOOST_KP;
    boostRect[0].regulator_Ki = BOOST_KI;
    boostRect[1].regulator_Kp = BOOST_KP;
    boostRect[1].regulator_Ki = BOOST_KI;
    boostRect[0].oV_setpoint = 0;
    boostRect[1].oV_setpoint = 0;
    boostRect[0].integral = 0;
    boostRect[1].integral = 0;

    regulator = 0;
	__enable_interrupt();
	__low_power_mode_4();

    volatile char data_buffer[128];
    volatile unsigned int param0, param1, boost_num;
	while(1){
	    if(usart_rx_get_line(data_buffer) == 1){
	        if(strstr(data_buffer, "REGULATOR") != NULL){
	            regulator = 1;
	            timerA_Start();
	        }else if(strstr(data_buffer, "MANUAL") != NULL){
	            regulator = 0;
	            timerA_Stop();
	        }
	        boost_num = decodeData(data_buffer, &param0, &param1);
	        if(boost_num != NULL){
	            if(regulator == 1){
	                BoostRect_RegulatorSetSetpoint(&boostRect[boost_num-1], param1);
	                BoostRect_ChangePwmParams(&boostRect[boost_num-1], boostRect[boost_num-1].pwm, param0);
	            }else if(regulator == 0){
	                BoostRect_ChangePwmParams(&boostRect[boost_num-1], param1, param0);
	            }
	        }
	        __low_power_mode_4();
	    }

	    if(adc_new_data == 1){
	        adc_new_data = 0;
	        BoostRect_ChangePwmParams(&boostRect[0], BoostRect_Regulator(&boostRect[0]), boostRect[0].pwm_freq_value);
	        BoostRect_ChangePwmParams(&boostRect[1], BoostRect_Regulator(&boostRect[1]), boostRect[1].pwm_freq_value);
	        __low_power_mode_4();
	    }



	}
}



/*
 *      ADC Start Conversion Interrupt
 *      40 Hz
 */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TimerA0_interrupt(void){
    adc_startConversion();
    TA0IV;
}
/*
 *      ADC End of Conversion Interrupt
 */

#pragma vector=ADC12_VECTOR
__interrupt void ADC_interrupt(void){
    boostRect[0].oV_actual = ADC12MEM0;
    boostRect[1].oV_actual = ADC12MEM1;
    adc_new_data = 1;
    __low_power_mode_off_on_exit();
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
     //   P9OUT &= ~BIT7;
        BoostRect_NegativeSign(&boostRect[1]);
    } else{
        P2IES |= BIT0;      //  change edge
    //    P9OUT |= BIT7;
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

/*
 *      UART Interrupt
 */

#pragma vector=USCI_A1_VECTOR
__interrupt void USART1_interrupt(void){
   if(UCA1IFG & UCTXIFG){
       usart_tx_interrupt_function();
   }
   if(UCA1IFG & UCRXIFG){
       usart_rx_interrupt_function();
   }
   __low_power_mode_off_on_exit();
}



unsigned int decodeData(char *data, unsigned int *param0, unsigned int *param1){
    unsigned int boostNumber = 0;
    char *string_begin = NULL;
    char *string_end = NULL;
    char tmp_buffer[32];
    unsigned int param0_tmp, param1_tmp;
    if(data == NULL){
        return 0;
    } else{
        string_begin =  strstr(data, "BOOST");
        string_end = strstr(data, "_");
        if(string_begin != NULL && string_end != NULL){
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

                memcpy(tmp_buffer, string_begin, string_end - string_begin);
                tmp_buffer[string_end - string_begin] = '\0';
                param0_tmp = atoi(tmp_buffer);
                string_begin = ++string_end;
                string_end = strstr(string_begin, "_");
                if(string_end == NULL){
                    return NULL;
                }

                memcpy(tmp_buffer, string_begin, string_end - string_begin);
                tmp_buffer[string_end - string_begin] = '\0';
                param1_tmp = atoi(tmp_buffer);
            }
        } else {
            return NULL;
        }
    }
    *param0 = param0_tmp;
    *param1 = param1_tmp;
    return boostNumber;
}

