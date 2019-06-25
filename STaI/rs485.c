/*
 * rs485.c
 *
 *  Created on: 12 gru 2017
 *      Author: rafal
 */
#include "rs485.h"

void usart_Init(void){
    /*
     *  USART0
     *   RXD <-> P2.5
     *   TXD <-> P2.4
     *
     */

    P2OUT |= BIT4;
    P2OUT &= ~BIT5;
    P2SEL |= BIT4 | BIT5;
    U0CTL |= CHAR | SWRST;          // 8 bit data. set reset state
    U0TCTL |= SSEL1;                // clock SMCLK = ~8MHz
    U0BR1 = 0x03;                   // baud rate
    U0BR0 = 0x69;                   // baud rate
    U0MCTL = 0x08;                  // baud rate
    U0CTL &= ~SWRST;                // release reset
    IE1 |= URXIE0;                  // RX interrupt
    ME1 |= UTXE0 | URXE0;           // usart enable
}

void rs485_Init(void){
    rs485_InitGPIO();
    rs485_ReceiveMode();
    usart_Init();
    rs485_RxCounter = 0;
    rs485_TxCounter = 0;
}
void rs485_ReceiveMode(void){
    rs485_ResetRE();
    rs485_ResetDE();
}
void rs485_TransmiterMode(void){
    rs485_SetDE();
    rs485_SetRE();
}
inline void rs485_InitGPIO(void){
    P2DIR |= BIT2 | BIT3;
}
inline void rs485_SetRE(void){
    P2OUT |= BIT2;
}
inline void rs485_SetDE(void){
    P2OUT |= BIT3;
}
inline void rs485_ResetRE(void){
    P2OUT &= ~BIT2;
}
inline void rs485_ResetDE(void){
    P2OUT &= ~BIT3;
}
inline void rs485_ClearRxBuffer(void){
    rs485_RxCounter = 0;
    rs485_RxBuffer[0] = '\0';
}
inline void usart_EnableTxInterrupt(void){
    IE1 |= UTXIE0;          // enable interrupt
    IFG1 |= UTXIFG0;        // set interrupt flag
}
inline void usart_DisableTxInterrupt(void){
    IE1 &= ~UTXIE0;
}
#pragma vector=USART0RX_VECTOR
__interrupt void usart0_rx_interrupt(void){
    rs485_RxBuffer[rs485_RxCounter] = U0RXBUF;
    rs485_RxBuffer[rs485_RxCounter+1] = '\0';
    rs485_RxCounter++;
    if(rs485_RxCounter > RS485_RX_BUFFER_LENGTH)
        rs485_RxCounter == 0;
}

#pragma vector=USART0TX_VECTOR
__interrupt void usart0_tx_interrupt(void){
    if(rs485_TxBuffer[rs485_TxCounter] == '\0'){
        rs485_TxCounter = 0;
        usart_DisableTxInterrupt();
        rs485_ReceiveMode();
    } else{
        U0TXBUF = rs485_TxBuffer[rs485_TxCounter];
        rs485_TxCounter++;
    }

}
