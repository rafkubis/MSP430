/*
 * usart.c
 *
 *  Created on: 3 gru 2018
 *      Author: rafal
 */
#include "usart.h"

char *tx_buffer;
char rx_buffer[RX_BUFFER_SIZE];

unsigned int tx_cnt;
unsigned int tx_available;
unsigned int rx_cnt;

void usart_init(void){
    rx_cnt = 0;
    UCA1CTLW0 |= UCSWRST;                       // hold reset state
    __usart_gpio_init();
    UCA1CTLW0 |= UCSSEL0 | UCSSEL1;             // clk SMCLK
    UCA1BRW = 52;                              // baud <-> INT N
    UCA1MCTLW = UCOS16 | 0x49 << 8 | 1 << 4;    // bayd <-> oversampling mode, second modulation stage,
                                                // first modulation stage

    UCA1CTLW0 &= ~UCSWRST;                      // release for operation
    UCA1IE |= UCRXIE;                           // rx interrupt enable
}

inline void __usart_enable_tx_interrupt(void){
    UCA1IE |= UCTXIE;
}

inline void __usart_disable_tx_interrupt(void){
    UCA1IE &= ~UCTXIE;
}

void usart_put_string(char *str){
    tx_available = 0;
    tx_buffer = str;
    tx_cnt = 0;                 // reset counter
    __usart_enable_tx_interrupt();
}

void usart_tx_interrupt_function(void){
    if(*(tx_buffer + tx_cnt) == '\0'){          // string end
        __usart_disable_tx_interrupt();
        tx_available = 1;
    }else{
        UCA1TXBUF = *(tx_buffer + tx_cnt);      // send character
        tx_cnt++;
    }
}

void usart_rx_interrupt_function(void){
    rx_buffer[rx_cnt] = UCA1RXBUF;
    rx_buffer[rx_cnt + 1] = '\0';
    if(rx_cnt < RX_BUFFER_SIZE - 1){
        rx_cnt++;
    }
}

void __usart_gpio_init(void){
    P3SEL0 |= BIT4 | BIT5; // alternate function
    P3SEL1 &= ~(BIT4 | BIT5);
}

unsigned int usart_tx_available(void){
    return tx_available;
}

unsigned int usart_rx_get_line(char *buffer){
    static char *end_of_line = NULL;
    static char *new_end_of_line = NULL;
    unsigned int length;
    if(end_of_line == NULL){
        end_of_line = strstr(rx_buffer, RX_END_OF_LINE);
        if(end_of_line == NULL){
            new_end_of_line == NULL;
            return 0;
        } else {
            *(buffer) = '\0';
            strncpy(buffer, rx_buffer, end_of_line - rx_buffer);
            *(buffer + (end_of_line - rx_buffer)) = '\0';
            if((end_of_line - rx_buffer + 1) == rx_cnt){        // to debug when RX_END_OF_line is "\l\n"
                rx_cnt = 0;
                rx_buffer[0] = '\0';
            }
            return 1;
        }
    } else {
        new_end_of_line = strstr(end_of_line + sizeof(RX_END_OF_LINE) - 1, RX_END_OF_LINE);
        if( new_end_of_line == NULL){
            return 0;
        } else {
            *buffer = '\0';
            end_of_line += sizeof(RX_END_OF_LINE) - 1;
            strncpy(buffer, end_of_line , new_end_of_line - end_of_line);
            *(buffer + (new_end_of_line - end_of_line)) = '\0';
            end_of_line = new_end_of_line;
            if((end_of_line - rx_buffer + 1) == rx_cnt){        // to debug when RX_END_OF_line is "\l\n"
                rx_cnt = 0;
                rx_buffer[0] = '\0';
            }
            return 1;
        }
    }
}





