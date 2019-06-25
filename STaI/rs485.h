/*
 * rs485.h
 *
 *  Created on: 12 gru 2017
 *      Author: rafal
 */

#ifndef RS485_H_
#define RS485_H_

#include <msp430fg439.h>
#include <string.h>

#define RS485_RX_BUFFER_LENGTH 64
#define RS485_TX_BUFFER_LENGTH 64

volatile unsigned int rs485_RxCounter;
volatile unsigned int rs485_TxCounter;
volatile char rs485_RxBuffer[RS485_RX_BUFFER_LENGTH+1];         // buffer length
volatile char rs485_TxBuffer[RS485_TX_BUFFER_LENGTH+1];         // + one character('\0' )

void usart_Init(void);
inline void usart_DisableTxInterrupt(void);
inline void usart_EnableTxInterrupt(void);


void rs485_Init(void);
inline void rs485_InitGPIO(void);
inline void rs485_SetRE(void);
inline void rs485_SetDE(void);
inline void rs485_ResetRE(void);
inline void rs485_ResetDE(void);
void rs485_ReceiveMode(void);
void rs485_TransmiterMode(void);
inline void rs485_ClearRxBuffer(void);
#endif /* RS485_H_ */
