/*
 * t_and_m_protocol.c
 *
 *  Created on: 12 gru 2017
 *      Author: rafal
 */
#include "mt_protocol.h"
#include "rs485.h"
#include <string.h>
#include <stdlib.h>

void mt_Init(void){
    rs485_Init();
 //   rs485_ReceiveMode();
}

int mt_checkCmdReceived(void){
    if( strchr(rs485_RxBuffer, '\r') == NULL){
        return 0;
    } else{
        return 1;
    }
}

int mt_decodeCmd(int *deviceAddress, char *command){
    char *cmdBegin = strchr(rs485_RxBuffer, '#');
    if(cmdBegin == NULL){
        return 0;
    }
    char deviceAddressString[2+1];
    deviceAddressString[0] = *(++cmdBegin);
    deviceAddressString[1] = *(++cmdBegin);
    deviceAddressString[2] = '\0';
    *deviceAddress = atoi(deviceAddressString);
    *command = *(++cmdBegin);
    rs485_ClearRxBuffer();
    return 1;
}

int mt_sendData(char *temperature, char *irradiance, char* ambient, char *wind){
    char *data[4];
    data[1] = temperature;
    data[0] = irradiance;
    data[2] = ambient;
    data[3] = wind;
    rs485_TxBuffer[0] = '\0';
    strcat(rs485_TxBuffer, "\r\n*");
    unsigned int i;
    strcat(rs485_TxBuffer, MT_DEVICE_ADDRESS);
    char string[2];
    string[0] = MT_CMD_DATA_REQUEST;
    string[1] = '\0';
    strcat(rs485_TxBuffer, string);
    for (i=0; i < 4; i++){
        strcat(rs485_TxBuffer, " ");
        strcat(rs485_TxBuffer, data[i]);
        strcat(rs485_TxBuffer, " ");
    }
    unsigned int buffer_length = strlen(rs485_TxBuffer);
    char crc = 0;
    for(i=2; i < buffer_length; i++){
        crc += rs485_TxBuffer[i];
    }
    string[0] = crc;
    string[1] = '\0';
    strcat(rs485_TxBuffer, string);
    strcat(rs485_TxBuffer, "\r\n");
    rs485_TransmiterMode();
    usart_EnableTxInterrupt();
    return 1;
}

int mt_sendHSVersion(void){
    rs485_TxBuffer[0] = '\0';
    strcat(rs485_TxBuffer, "\r\n*");
    strcat(rs485_TxBuffer, MT_DEVICE_ADDRESS);
    char string[2];
    string[0] = MT_CMD_H_S_VER;
    string[1] = '\0';
    strcat(rs485_TxBuffer, string);
    strcat(rs485_TxBuffer, MT_HARDWARE_VER);
    strcat(rs485_TxBuffer, MT_SOFTWARE_VER);
    strcat(rs485_TxBuffer, "\n");
    rs485_TransmiterMode();
    usart_EnableTxInterrupt();
    return 1;
}

int mt_sendRecognition(void){
    rs485_TxBuffer[0] = '\0';
    strcat(rs485_TxBuffer, "\r\n*");
    strcat(rs485_TxBuffer, MT_DEVICE_ADDRESS);
    strcat(rs485_TxBuffer, "7");
    strcat(rs485_TxBuffer, "\n");
    rs485_TransmiterMode();
    usart_EnableTxInterrupt();
    return 1;
}

