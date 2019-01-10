/*
 * bridge_boost.c
 *
 *  Created on: 8 sty 2019
 *      Author: rafal
 */

#include "bridge_boost.h"


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
        *(boostRect->T2_pwm_register) = boostRect->pwm;
        *(boostRect->T1_pwm_register) = boostRect->pwm_max+1;
    }else{
        *(boostRect->T1_pwm_register) = boostRect->pwm;
        *(boostRect->T2_pwm_register) = boostRect->pwm_max+1;
    }

}

inline unsigned int BoostRect_Regulator(BoostRect *boostRect){
    static volatile int integral = 0;
    volatile signed long I = 0;
    volatile signed long P = 0;
    volatile long pwm_tmp;
    volatile int error = boostRect->oV_setpoint - boostRect->oV_actual;
  //  P = 1 * error;
    P = (signed long)(boostRect->regulator_Kp) * (signed long)error;
    integral += error;
    if(integral > 500 ){
        integral = 500;
    }else if(integral < -500){
        integral = -500;
    }
    I = (signed long)(boostRect->regulator_Ki) * (signed long)integral;
    pwm_tmp = P + I;
    if(pwm_tmp < 0){
        pwm_tmp = 0;
    } else{
        pwm_tmp /= 256;
        if(pwm_tmp > BOOSTRECT_PWM_MAX_DUTY){
            pwm_tmp = BOOSTRECT_PWM_MAX_DUTY;
        }
    }
    if(pwm_tmp > BOOSTRECT_PWM_MAX_DUTY){
        pwm_tmp = BOOSTRECT_PWM_MAX_DUTY;
    }
    return pwm_tmp;
}



