/*
 * rtc.h
 *
 * Created: 31.05.2017 11:50:23
 *  Author: asinn
 */ 


#ifndef RTC_H_
#define RTC_H_

#include "config.h"


typedef void (*callback_t) (void);

void rtc_init();
void set_second_callback(callback_t callback);
uint32_t rtc_get_time(void);



#endif /* RTC_H_ */