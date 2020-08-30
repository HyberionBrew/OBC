/**
* @file hal_pwm.h
* @author Jan Pac, Andreas Sinn
* @date 19 January 2020
* @brief The PWM HAL interface header file for the MTB
*/


#ifndef SOFT_PWM_H_
#define SOFT_PWM_H_

#include "config.h"

void mt_init(void);
void mt_enable(void);
void mt_disable(void);
void soft_pwm_init(void);
void soft_pwm_enable(void);
void soft_pwm_disable(void);
void soft_pwm_update_duty_cycle(int8_t x, int8_t y, int8_t z);

#endif /* SOFT_PWM_H_ */