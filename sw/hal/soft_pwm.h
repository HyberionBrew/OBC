/*
 * soft_pwm.h
 *
 * Created: 10.07.2017 16:05:36
 *  Author: asinn
 */ 


#ifndef SOFT_PWM_H_
#define SOFT_PWM_H_

#include "config.h"

void soft_pwm_init(void);
void mt_update_duty_cycle(int8_t x, int8_t y, int8_t z);

#endif /* SOFT_PWM_H_ */