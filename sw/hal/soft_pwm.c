/*
* soft_pwm.c
*
* Created: 10.07.2017 16:05:23
*  Author: asinn
*/

#include "soft_pwm.h"

#define SOFT_PWM_FREQUENCY	93
#define SOFT_PWM_STEPS		64

#define SOFT_PWM_OCR (((F_CPU) / 64) / (SOFT_PWM_FREQUENCY) - 1)	// ~170

#if (SOFT_PWM_OCR > 255)
#error  "Soft PWM frequency too low!"
#elif (SOFT_PWM_OCR < 14)
#error "Soft PWM frequency too high!"
#endif

typedef struct mt_s
{
	int8_t cx;
	int8_t cy;
	int8_t cz;
	uint8_t cc;
} volatile mt_t;

static mt_t mt;

void soft_pwm_init(void)
{
	// Configure PWM pins as output
	DDR(MT_PWM_R1_PORT) |= (1<<MT_PWM_R1_PIN);
	DDR(MT_PWM_F1_PORT) |= (1<<MT_PWM_F1_PIN);
	DDR(MT_PWM_R2_PORT) |= (1<<MT_PWM_R2_PIN);
	DDR(MT_PWM_F2_PORT) |= (1<<MT_PWM_F2_PIN);
	DDR(MT_PWM_R3_PORT) |= (1<<MT_PWM_R3_PIN);
	DDR(MT_PWM_F3_PORT) |= (1<<MT_PWM_F3_PIN);
	
	// Clear all PWM pins
	PORT(MT_PWM_R1_PORT) &= ~(1<<MT_PWM_R1_PIN);
	PORT(MT_PWM_F1_PORT) &= ~(1<<MT_PWM_F1_PIN);
	PORT(MT_PWM_R2_PORT) &= ~(1<<MT_PWM_R2_PIN);
	PORT(MT_PWM_F2_PORT) &= ~(1<<MT_PWM_F2_PIN);
	PORT(MT_PWM_R3_PORT) &= ~(1<<MT_PWM_R3_PIN);
	PORT(MT_PWM_F3_PORT) &= ~(1<<MT_PWM_F3_PIN);
		
	// Initialize timer module
	TCCR3A = (1<<WGM31); // CTC Modus
	TCCR3B |= (1<<CS21) | (1<<CS20); // Prescaler: Sysclock/64
	
	TIMSK3 |= (1<<OCIE3A); // Enable compare interrupt
	
	OCR3A = SOFT_PWM_OCR; // Defined by SOFT_PWM_FREQUENCY
}

void mt_update_duty_cycle(int8_t x, int8_t y, int8_t z)
{
	mt.cx = x;
	mt.cy = y;
	mt.cz = z;
}
 
ISR (TIMER3_COMPA_vect)	// triggered every time the timer passes the OCR3A value (top value)! 
{
	mt.cc++;	// Initialize mt.cc to zero at beginning?
	
	if(mt.cc > SOFT_PWM_STEPS)
	{
		mt.cc = 0;
	}
		
	
	if (mt.cc == 0)
	{
		if(mt.cx > 0)
		{
			PORT(MT_PWM_F1_PORT) |= (1<<MT_PWM_F1_PIN);
		}
		else if(mt.cx < 0)
		{
			PORT(MT_PWM_R1_PORT) |= (1<<MT_PWM_R1_PIN);
		}
		
		if(mt.cy > 0)
		{
			PORT(MT_PWM_F2_PORT) |= (1<<MT_PWM_F2_PIN);
		}
		else if(mt.cy < 0)
		{
			PORT(MT_PWM_R2_PORT) |= (1<<MT_PWM_R2_PIN);
		}
		
		if(mt.cz > 0)
		{
			PORT(MT_PWM_F3_PORT) |= (1<<MT_PWM_F3_PIN);
		}
		else if(mt.cz < 0)
		{
			PORT(MT_PWM_R3_PORT) |= (1<<MT_PWM_R3_PIN);
		}
	}
	
	
	if(mt.cc == abs(mt.cx))
	{
		PORT(MT_PWM_R1_PORT) &= ~(1<<MT_PWM_R1_PIN);	
		PORT(MT_PWM_F1_PORT) &= ~(1<<MT_PWM_F1_PIN);
	}
	
	if(mt.cc == abs(mt.cy))
	{
		PORT(MT_PWM_R2_PORT) &= ~(1<<MT_PWM_R2_PIN);
		PORT(MT_PWM_F2_PORT) &= ~(1<<MT_PWM_F2_PIN);
	}
	
	if(mt.cc == abs(mt.cz))
	{
		PORT(MT_PWM_R3_PORT) &= ~(1<<MT_PWM_R3_PIN);
		PORT(MT_PWM_F3_PORT) &= ~(1<<MT_PWM_F3_PIN);
	}
}