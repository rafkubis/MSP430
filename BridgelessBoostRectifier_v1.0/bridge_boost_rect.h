/*
 * bridge_boost_rect.h
 *
 *  Created on: 20 paü 2018
 *      Author: rafal
 */
#include <msp430.h>

#ifndef BRIDGE_BOOST_RECT_H_
#define BRIDGE_BOOST_RECT_H_

#define BOOSTRECT_PWM_MAX           160
#define BOOSTRECT_PWM_MAX_DUTY      144     // max 90%

typedef struct BoostRect{
	unsigned int pwm;
	unsigned int pwm_max;
	volatile unsigned int *T1_pwm_register;
	volatile unsigned int *T2_pwm_register;
	volatile unsigned int *pwm_register;
	unsigned int oV_actual;         // output voltage
	unsigned int oV_setpoint;       // output voltage setpoint
	unsigned int regulator_Kp;
}BoostRect;

inline void BoostRect_PositiveSign(BoostRect *boostRect);
inline void BoostRect_NegativeSign(BoostRect *boostRect);
inline void BoostRect_ChangePwmParams(BoostRect *boostRect,unsigned int pwm, unsigned int pwm_max);

#endif /* BRIDGE_BOOST_RECT_H_ */
