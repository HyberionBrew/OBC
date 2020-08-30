/*
 * rtc.c
 *
 * Created: 31.05.2017
 *  Author: asinn
 */ 

#include "rtc.h"

uint32_t utc_time;
uint32_t seconds_offset_to_UTC;


callback_t second_callback;

		//sleep_mode();										//Enter sleep mode. (Will wake up from timer overflow interrupt)
		//TCCR0=(1<<CS00)|(1<<CS02);							//Write dummy value to control register
		//while(ASSR&((1<<TCN0UB)|(1<<OCR0UB)|(1<<TCR0UB)));	//Wait until TC0 is updated


void rtc_init(void)
{
	uint8_t counter = 0;
	

	TIMSK2 &= ~((1<<TOIE2)|(1<<OCIE2A)|(1<<OCIE2B));		// Disable all TC0 isabled
	ASSR |= (1<<AS2);										// Asyncrhonous timer/counter2 with external clock (32.768 kHz)
	TCNT2 = 0;												// Reset timer
	TCCR2B =(1<<CS20)|(1<<CS22);								// Prescale to 1 interrupt every second
	
	
	while (ASSR & ((1<<TCN2UB)|(1<<OCR2AUB)|(1<<TCR2AUB)))	// Wait until TC0 is updated
	{
		_delay_ms(100);
		if(counter > 20)
		{
			// Error occured
			// Todo: error handling?
			break;
		}
		counter++;
	}
	
	// Todo: get offset from non-volatile storage
	utc_time = seconds_offset_to_UTC;
	second_callback = NULL;
	
	TIMSK2 |= (1<<TOIE2);									// Enable interrupt
	
	//sei();												// Set the Global Interrupt Enable Bit
	//set_sleep_mode(SLEEP_MODE_PWR_SAVE);					// Selecting power save mode as the sleep mode to be used
	//sleep_enable();										// Enabling sleep mode
}

void set_second_callback(callback_t callback)
{
	second_callback = callback;
}

uint32_t rtc_get_time(void)
{
	return utc_time;
}

ISR(TIMER2_OVF_vect)
{
	// Interrupt called by RTC every second
	utc_time++;
	
	// Store utc_time every hour
	// if utc_time > stored time && utc_time - stored time < 3600s -> store, else use stored value
	
	
	if(second_callback != NULL)
	{
		second_callback();
	}
}

