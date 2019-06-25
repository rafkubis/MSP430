/*
 * ds18b20.h
 *
 *  Created on: 29 gru 2017
 *      Author: rafal
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "one_wire.h"


void ds18b20_startConversion(void);
void ds18b20_readTemperature(unsigned int *sign, unsigned int *integer, unsigned int *fraction);

#endif /* DS18B20_H_ */
