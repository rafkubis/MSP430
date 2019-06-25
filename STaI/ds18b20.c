/*
 * ds18b20.c
 *
 *  Created on: 29 gru 2017
 *      Author: rafal
 */
#include "ds18b20.h"
unsigned int fractionVaues[4] = {625, 1250, 2500, 5000};

void ds18b20_startConversion(void){
    oneWire_checkPresence();
    oneWire_sendByte(0xcc);         // Skip ROM
    oneWire_sendByte(0x44);         // Convert T
}

void ds18b20_readTemperature(unsigned int *sign, unsigned int *integer, unsigned int *fraction){
    oneWire_checkPresence();
    oneWire_sendByte(0xcc);                              // Skip ROM
    oneWire_sendByte(0xbe);                             // Read Scratchpad
    unsigned int temperature = 0;
    unsigned int data = 0;
    temperature = oneWire_readByte();
    temperature |= oneWire_readByte() << 8;
    if( 0xf800 & temperature)                        // bit 15 - 11 - sign
        *sign = 1;
    else
        *sign = 0;
    data = (0x07f0 & temperature) >> 4;              // bit 10 - bit 4  - integer value
    *integer = data;
    data = 0;
    data = 0x000f & temperature;                     // bit 3 - bit 0  - fravtion value
    int i;
    *fraction = 0;
    for(i=0; i<4; i++){
        if( data & 1<<i){
            *fraction += fractionVaues[i];
        }
    }

}
