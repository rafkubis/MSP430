/*
 * one_wire.h
 *
 *  Created on: 28 gru 2017
 *      Author: rafal
 */

#ifndef ONE_WIRE_H_
#define ONE_WIRE_H_

#include <msp430.h>

/*
 *      One wire port definitions
 */
#define ONE_WIRE_PORT_DIR   P4DIR
#define ONE_WIRE_PORT_OUT   P4OUT
#define ONE_WIRE_PORT_IN    P4IN
#define ONE_WIRE_PIN        BIT5
/*
 *      One Wire delays const
 *      fclk = 32 * 8 * 32.768kHz = 8388.6 kHz
 */
#define ONE_WIRE_480_US 440
#define ONE_WIRE_10_US  5
#define ONE_WIRE_25_US  16
#define ONE_WIRE_50_US  36
#define ONE_WIRE_DEVICE_PRESENT     1
#define ONE_WIRE_DEVICE_NOT_PRESENT 0

inline void oneWire_dqInput(void);
inline void oneWire_dqOutput(void);
inline void oneWire_dqSet(void);
inline void oneWire_dqReset(void);
inline int oneWire_dqReadInput(void);
void oneWire_wait(unsigned int us);
int oneWire_checkPresence(void);
void oneWire_sendByte(unsigned int byte);
unsigned int oneWire_readByte(void);

#endif /* ONE_WIRE_H_ */
