/*
 * mt_protocol.h
 *
 *  Created on: 16 gru 2017
 *      Author: rafal
 */

#ifndef MT_PROTOCOL_H_
#define MT_PROTOCOL_H_

#include <msp430fg439.h>
#include "rs485.h"

#define MT_DEVICE_ADDRESS              "01"
#define MT_HARDWARE_VER                "100"
#define MT_SOFTWARE_VER                "200"

#define MT_CMD_DEVICE_RECOGNITION       '0'
#define MT_CMD_DATA_REQUEST             '7'
#define MT_CMD_H_S_VER                  'v'

void mt_Init(void);
int mt_checkCmdReceived(void);
int mt_decodeCmd(int *deviceAddress, char *command);
int mt_sendData(char *temperature, char *irradiance, char *ambient, char *wind);
int mt_sendHSVersion(void);
int mt_sendRecognition(void);

#endif /* MT_PROTOCOL_H_ */
