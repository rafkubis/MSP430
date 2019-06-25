/*
 * one_wire.c
 *
 *  Created on: 28 gru 2017
 *      Author: rafal
 */
#include "one_wire.h"


inline void oneWire_dqInput(void){
    ONE_WIRE_PORT_DIR &= ~ONE_WIRE_PIN;
}
inline void oneWire_dqOutput(void){
    ONE_WIRE_PORT_DIR |= ONE_WIRE_PIN;
}
inline void oneWire_dqSet(void){
    ONE_WIRE_PORT_OUT |= ONE_WIRE_PIN;
}
inline void oneWire_dqReset(void){
    ONE_WIRE_PORT_OUT &= ~ONE_WIRE_PIN;
}
inline int oneWire_dqReadInput(void){
    return  ONE_WIRE_PORT_IN & ONE_WIRE_PIN;
}
void oneWire_wait(unsigned int us){
    volatile unsigned int i;
    for (i =us ; i > 0 ; i--);
}


int oneWire_checkPresence(void){
    oneWire_dqOutput();
    oneWire_dqReset();
    oneWire_wait(ONE_WIRE_480_US);
    oneWire_dqSet();
    oneWire_dqInput();
    int zeroCounter = 0;
    int i;
    for(i=0; i<10; i++){
        oneWire_wait(ONE_WIRE_25_US);
    //    tab[i] = oneWire_dqReadInput();
        if(oneWire_dqReadInput() == 0){
            zeroCounter++;
        }

    }
    oneWire_wait(ONE_WIRE_50_US);
    if(zeroCounter > 1){
        return ONE_WIRE_DEVICE_PRESENT;
    } else{
        return ONE_WIRE_DEVICE_NOT_PRESENT;
    }
}

void oneWire_sendByte(unsigned int byte){
    int i;
    oneWire_dqOutput();
    for(i=0; i<8; i++){
        if(byte & 1<<i){
            oneWire_dqReset();
            oneWire_wait(ONE_WIRE_10_US);
            oneWire_dqSet();
            oneWire_wait(ONE_WIRE_50_US);
            oneWire_dqSet();
            oneWire_wait(ONE_WIRE_25_US);
        } else{
            oneWire_dqReset();
            oneWire_wait(ONE_WIRE_10_US);
            oneWire_dqReset();
            oneWire_wait(ONE_WIRE_50_US);
            oneWire_dqSet();
            oneWire_wait(ONE_WIRE_25_US);
        }
    }
    oneWire_dqSet();
}
unsigned int oneWire_readByte(void){
    unsigned int byte = 0;
    int i,j;
    int low = 0;

    for(i=0; i<8; i++){
        oneWire_dqOutput();
        oneWire_dqReset();
        oneWire_wait(ONE_WIRE_10_US);
        oneWire_dqSet();
        oneWire_dqInput();
        for(j=0; j<5; j++){
            oneWire_wait(ONE_WIRE_10_US);
            if(oneWire_dqReadInput()==0){
                low++;
            }
        }
        if(low == 0){
            byte |= 1<<i;
        }
        low = 0;
        oneWire_wait(ONE_WIRE_25_US);

    }
    oneWire_wait(ONE_WIRE_50_US);
    return byte;
}


