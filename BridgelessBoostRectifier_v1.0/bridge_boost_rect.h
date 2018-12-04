/*
 * bridge_boost_rect.h
 *
 *  Created on: 20 paü 2018
 *      Author: rafal
 */
#include <msp430.h>

#ifndef BRIDGE_BOOST_RECT_H_
#define BRIDGE_BOOST_RECT_H_

typedef struct BoostRect{
	unsigned int pwm;
	unsigned int pwm_max;
	volatile unsigned int *T1_pwm_register;
	volatile unsigned int *T2_pwm_register;
//	volatile unsigned int *pwm_register;
}BoostRect;

inline void BoostRect_PositiveSign(BoostRect *boostRect);
inline void BoostRect_NegativeSign(BoostRect *boostRect);
//inline void BoostRect_ChangePwmPrams(BoostRect *boostRect,unsigned int pwm, unsigned int pwm_max);

#endif /* BRIDGE_BOOST_RECT_H_ */
