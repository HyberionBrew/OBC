/**
* @file hal_pwm.c
* @author Jan Pac, Andreas Sinn
* @date 19 January 2020
* @brief The PWM HAL interface for the ATMEGA1280L
*
* @details interface for enabling the MTB (magnetic torquer board) and starting the PWM for controlling the three magnetic coils
*/

// ------------------------ Adapted from Andi's code -------------------------------
#include "hal_pwm.h"

#define SOFT_PWM_FREQUENCY	93
#define SOFT_PWM_STEPS		64

#define SOFT_PWM_OCR (((F_CPU) / 64) / (SOFT_PWM_FREQUENCY) - 1)	// ~170

#if (SOFT_PWM_OCR > 255)
#error  "Soft PWM frequency too low!"
#elif (SOFT_PWM_OCR < 14)
#error "Soft PWM frequency too high!"
#endif

typedef struct mt_s		// JP: I treat this as the (already scaled) input for the duty cycle of the PWM; changing duty cycle will change the applied voltage to the coils!
{						
	int8_t cx;
	int8_t cy;
	int8_t cz;
	uint8_t cc;
} volatile mt_t;

static mt_t mt;
// ------------------------ END: Adapted from Andi's code ----------------------------

void mt_init(void)
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
	
	// Disables MTB
	mt_disable();
}

void mt_enable(void)
{
	PORT(MT_ENABLE_PORT) |= (1<<MT_ENABLE_PIN);
}

void mt_disable(void)
{
	PORT(MT_ENABLE_PORT) &= ~(1<<MT_ENABLE_PIN);
}

void soft_pwm_init(void)
{
	//might need to enable intterupts
	sei();
	//Set Power reduction register of timer 3 to 0
	PRR1 &= ~(1<<PRTIM3);
	
	// Set duty cycles for testing
	mt.cx = 10;
	mt.cy = 20;
	mt.cz = 30;
	
	// Initialize counting parameter to zero
	mt.cc = 0;
	
	// Initialize timer module; adapted from Andi
	TCCR3A |= (1<<WGM31); // CTC Modus
	TCCR3B |= (1<<CS21) | (1<<CS20); // Prescaler: Sysclock/64
	
	TIMSK3 |= (1<<OCIE3A); // Enable compare interrupt
	
	OCR3A = SOFT_PWM_OCR; // Defined by SOFT_PWM_FREQUENCY; top value
}

void soft_pwm_enable(void)
{
	TCCR3A |= (1<<WGM31);	// Sets timer3 back to CTC mode!
	mt.cc = 0;				// Initialize counting parameter to zero
}

void soft_pwm_disable(void)
{
	TCCR3A &= ~(1<<WGM31);	// Disables CTC mode!
	mt.cc = 0;				// Set counting parameter to zero
}

void soft_pwm_update_duty_cycle(int8_t x, int8_t y, int8_t z)
{	
	// Clear all PWM pins
	PORT(MT_PWM_R1_PORT) &= ~(1<<MT_PWM_R1_PIN);
	PORT(MT_PWM_F1_PORT) &= ~(1<<MT_PWM_F1_PIN);
	PORT(MT_PWM_R2_PORT) &= ~(1<<MT_PWM_R2_PIN);
	PORT(MT_PWM_F2_PORT) &= ~(1<<MT_PWM_F2_PIN);
	PORT(MT_PWM_R3_PORT) &= ~(1<<MT_PWM_R3_PIN);
	PORT(MT_PWM_F3_PORT) &= ~(1<<MT_PWM_F3_PIN);
	
	// Set new duty cycles
	mt.cx = x;
	mt.cy = y;
	mt.cz = z;
}

ISR (TIMER3_COMPA_vect)
{
	mt.cc++;
	
	if(mt.cc > SOFT_PWM_STEPS)	// Watch out: This means that cx, cy and cz have to be smaller than SOFT_PWM_STEPS!
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


// add doxygen function descriptions!


