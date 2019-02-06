/*
 * bridge_boost.c
 *
 *  Created on: 8 sty 2019
 *      Author: rafal
 */

#include "bridge_boost.h"


inline void BoostRect_PositiveSign(BoostRect *boostRect){
    *(boostRect->T1_pwm_register) = boostRect->pwm_freq_value+1;
    *(boostRect->T2_pwm_register) = boostRect->pwm;
}

inline void BoostRect_NegativeSign(BoostRect *boostRect){
    *(boostRect->T1_pwm_register) = boostRect->pwm;
    *(boostRect->T2_pwm_register) = boostRect->pwm_freq_value+1;
}

inline void BoostRect_ChangePwmParams(BoostRect *boostRect,unsigned int pwm, unsigned int pwm_freq_value){
    *(boostRect->pwm_register) = pwm_freq_value;
    boostRect->pwm_freq_value = pwm_freq_value;
    boostRect->pwm = pwm;
    boostRect->pwm_max = (pwm_freq_value * 90) / 100;

    if(*(boostRect->T1_pwm_register) == boostRect->pwm_freq_value+1){
        *(boostRect->T2_pwm_register) = boostRect->pwm;
        *(boostRect->T1_pwm_register) = boostRect->pwm_freq_value+1;
    }else{
        *(boostRect->T1_pwm_register) = boostRect->pwm;
        *(boostRect->T2_pwm_register) = boostRect->pwm_freq_value+1;
    }

}

inline void BoostRect_ChangePwmFreqParam(BoostRect *boostRect, unsigned int pwm_freq_value){
    boostRect->pwm_freq_value = pwm_freq_value;
}

inline unsigned int BoostRect_Regulator(BoostRect *boostRect){
    volatile signed long I = 0;
    volatile signed long P = 0;
    volatile long pwm_tmp;
    volatile int error = boostRect->oV_setpoint - boostRect->oV_actual;
  //  P = 1 * error;
    P = (signed long)(boostRect->regulator_Kp) * (signed long)error;
    boostRect->integral += error;
    if(boostRect->integral > 500 ){
        boostRect->integral = 500;
    }else if(boostRect->integral < -500){
        boostRect->integral = -500;
    }
    I = (signed long)(boostRect->regulator_Ki) * (signed long)boostRect->integral;
    pwm_tmp = P + I;
    if(pwm_tmp < 0){
        pwm_tmp = 0;
    } else{
        pwm_tmp /= 256;
        if(pwm_tmp > boostRect->pwm_max){
            pwm_tmp = boostRect->pwm_max;
        }
    }
    /*
    if(pwm_tmp > BOOSTRECT_PWM_MAX_DUTY){
        pwm_tmp = BOOSTRECT_PWM_MAX_DUTY;
    }
    */
    return pwm_tmp;
}

inline void BoostRect_RegulatorSetSetpoint(BoostRect *boostRect, unsigned int setPoint){
    boostRect->oV_setpoint = setPoint;
}


