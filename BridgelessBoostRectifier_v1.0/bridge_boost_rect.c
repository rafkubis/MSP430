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

inline void BoostRect_ChangePwmParams(BoostRect *boostRect,unsigned int pwm, unsigned int pwm_max){
    *(boostRect->pwm_register) = pwm_max;
    boostRect->pwm_max = pwm_max;
    boostRect->pwm = pwm;

    if(*(boostRect->T1_pwm_register) == boostRect->pwm_max+1){
        *(boostRect->T1_pwm_register) = boostRect->pwm;
        *(boostRect->T2_pwm_register) = boostRect->pwm_max+1;
    }else{
        *(boostRect->T1_pwm_register) = boostRect->pwm;
        *(boostRect->T2_pwm_register) = boostRect->pwm_max+1;
    }

}

void BoostRect_Regulator(BoostRect *boostRect){
    int error = boostRect->oV_setpoint - boostRect->oV_actual;
    int pwm_tmp = Kp * error;
    pwm_tmp += boostRect->pwm;
    if(pwm_tmp > BOOSTRECT_PWM_MAX_DUTY){
        pwm_tmp = BOOSTRECT_PWM_MAX_DUTY;
    } else if(pwm_tmp < 0){
        pwm_tmp = 0;
    }
    BoostRect_ChangePwmParams(boostRect, pwm_tmp, BOOSTRECT_PWM_MAX);
}
