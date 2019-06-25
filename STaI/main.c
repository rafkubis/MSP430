#include <msp430.h> 
#include "clock.h"
#include "ds18b20.h"
#include "mt_protocol.h"
#include "adc.h"
#include "stdlib.h"
/*
 * main.c
 */

void temperatureToString(int sign, int integer, int fraction, char *string);
void adcResultToIrradianceString(unsigned int result, char *string);
int itoa(int value, char *sp, int radix);

const unsigned int LSB_uV = 366;
char temperature_string[10];
char irradiance_string[10];
unsigned int temperature_sign, temperature_integer, temperature_fraction;
int received_device_address;
char received_cmd;
int device_address;

void main(void) {
    device_address = atoi(MT_DEVICE_ADDRESS);            // device address to compare in infinte loop
    received_cmd = 0xff ;                               // no cmd received
    received_device_address = 100;                      // device address available from 0 to 99

    WDTCTL = WDTPW | WDTHOLD;	                        // Stop watchdog timer
    clock_8MHz();
    mt_Init();
    adc_init();
    _enable_interrupts();


    while(1){
        if(mt_checkCmdReceived() == 1 ){
            mt_decodeCmd(&received_device_address, &received_cmd);

            if(received_device_address == device_address){

                switch(received_cmd){
                case MT_CMD_DEVICE_RECOGNITION:
                    mt_sendRecognition();
                    received_cmd = '\0';
                    break;
                case MT_CMD_H_S_VER:
                    mt_sendHSVersion();
                    received_cmd = '\0';
                    break;
                case MT_CMD_DATA_REQUEST:
                    ds18b20_startConversion();
                    adc_startConversion();
                    clock_wait(0xffff);
                    ds18b20_readTemperature(&temperature_sign, &temperature_integer, &temperature_fraction);
                    temperatureToString(temperature_sign, temperature_integer, temperature_fraction, temperature_string);
                    while(adc_checkBusy());
                    adcResultToIrradianceString(adc_readData(), irradiance_string);
                    mt_sendData(temperature_string, irradiance_string, "0.0", "0.0");
                    break;
                default:
                    received_cmd = '\0';
                }
            }else {
                received_device_address = 100;
            }



        }
    }

}


void temperatureToString(int sign, int integer, int fraction, char *string){
    if(sign == 1){
        *string = '-';
        string++;
        *string = '\0';
    }else{
        *string = '\0';
    }
    char value_string[4];
    itoa(integer, value_string, 10);
    strcat(string, value_string);
    value_string[0] = 0x2e;
    value_string[1] = '\0';
    strcat(string, value_string);
    itoa(fraction, value_string, 10);
    value_string[1] = '\0';
    strcat(string, value_string);

}

void adcResultToIrradianceString(unsigned int result, char *string){
     volatile unsigned long current_uA;
     current_uA = (((unsigned long)result * (unsigned long)LSB_uV) / ADC_OPAMP_GAIN ) * 10;
     unsigned int irradiance = current_uA/1000;
     unsigned int irradiance_fraction = current_uA % 1000;
     char irradinace_fraction_string[10];
     char dot_string[2];
     dot_string[0] = 0x2e;                  // dot ASCII
     dot_string[1] = '\0';
     itoa(irradiance, string, 10);
     itoa(irradiance_fraction, irradinace_fraction_string, 10);
     irradinace_fraction_string[1] = '\0';
     strcat(string, dot_string);
     strcat(string, irradinace_fraction_string);
}

// Implementation of itoa()
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

    *sp = '\0';
    return len;
}

