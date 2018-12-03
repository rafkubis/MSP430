/*
 * bridge_boost_rect.c
 *
 *  Created on: 20 paŸ 2018
 *      Author: rafal
 */
#include "bridge_boost_rect.h"


inline void BoostRect_PositiveSign(BoostRect *boostRect){
	*(boostRect->T1_pwm_register) = boostRect->pwm_max+1;
	*(boostRect->T2_pwm_register) = boostRect->pwm;
}

inline void BoostRect_NegativeSign(BoostRect *boostRect){
	*(boostRect->T1_pwm_register) = boostRect->pwm;
	*(boostRect->T2_pwm_register) = boostRect->pwm_max+1;
}
