/*
 * usart.h
 *
 *  Created on: 3 gru 2018
 *      Author: rafal
 */
#include <msp430.h>
#include "string.h"
#ifndef USART_H_
#define USART_H_

#define RX_BUFFER_SIZE 256
#define RX_END_OF_LINE "\r"
//#define RX_END_OF_LINE "\l\n"

void usart_init(void);
void usart_put_string(char *str);
inline void usart_tx_interrupt_function(void);
inline void usart_rx_interrupt_function(void);
unsigned int usart_tx_available(void);
unsigned int usart_rx_get_line(char *buffer);

void __enable_tx_interrupt(void);
void __usart_gpio_init(void);
void __usart_enable_tx_interrupt(void);
void __usart_disable_tx_interrupt(void);

#endif /* USART_H_ */
