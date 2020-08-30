/*
 * config.h
 *
 * Created: 22.11.2016 14:38:52
 *  Author: asinn
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_


// --- Includes ---
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <stdbool.h>
#include <stdlib.h>

#include "git_version.h"


#define HW_VER_EM2_0	// Available hardware versions: EM2_0
#include "hw_defs.h"

#define EEPROM_FORCE_REPROGRAMMING 0

#if 0
FUSES =
{
	.low = 0xE1,		// Internal calibrated RC-oscillator @ 1MHz
	.high = 0x91,		
	.extended = 0xFF,	
};
#endif 


typedef enum
{
	DONE = 0, FAILED = !DONE
} RetVal;


void mt_init(void);
bool mt_enable(void);
bool mt_disable(void);
bool mt_state(void);




void vee_init(void);
void vdd_init(void);
void rs485_tx_nrx_init(void);




// --- Preprocessor macros ---
#define CONCAT(a,b)		a##b
#define DDR(y)			CONCAT(DDR, y)
#define PORT(y)			CONCAT(PORT, y)
#define PIN(y)			CONCAT(PIN, y)
#define LOG_1(n) (((n) >= 2) ? 1 : 0)
#define LOG_2(n) (((n) >= 1<<2) ? (2 + LOG_1((n)>>2)) : LOG_1(n))

typedef enum 
{
	DISABLE = 0,
	ENABLE
	} state_e;

// ATTINY841 values
#define CRYSTAL_OSZILLATOR_FREQ	7372800


// Clock frequency settings
#define CLOCK_USED (CRYSTAL_OSZILLATOR_FREQ)
#define CPU_CLOCK_PRESCALER 8

//#define ADC_VREF 4.096
//#define ADC_RES	((float) ADC_VREF/1023.0)

typedef enum rbf_state_e
{
	NOT_INSERTED = 0,
	INSERTED
	} rbf_state;

#if (EEPROM_FORCE_REPROGRAMMING)
#warning "EEPROM forced reprogramming is activated!"
#endif

#define WATCHDOG_TIMEOUT 4 // s

#define RADIO_BAUD_RATE 9600

#if (CLOCK_USED / CPU_CLOCK_PRESCALER) != F_CPU
#error "Desired CPU clock frequency is not equal to set F_CPU!"
#endif

void vdd_disable(void);
void vdd_enable(void);
void vee_enable(void);
void vee_disable(void);

inline void vdd_pin_enable(void)
{
	PORT(VDD_DISABLE_PORT) &= ~(1<<VDD_DISABLE_PIN);
}

inline void vdd_pin_disable(void)
{
	PORT(VDD_DISABLE_PORT) |= (1<<VDD_DISABLE_PIN);
}

inline void vee_pin_enable(void)
{
	PORT(VEE_DISABLE_PORT) &= ~(1<<VEE_DISABLE_PIN);
}

inline void vee_pin_disable(void)
{
	PORT(VEE_DISABLE_PORT) |= (1<<VEE_DISABLE_PIN);
}

inline bool vdd_is_disabled(void)
{
	return (PORT(VDD_DISABLE_PORT) | (1<<VDD_DISABLE_PIN));
}

inline void rs485_tx_nrx_enable(void)
{
	PORT(RS485_TX_NRX_PORT) |= (1<<RS485_TX_NRX_PIN);
}

inline void rs485_tx_nrx_disable(void)
{
	PORT(RS485_TX_NRX_PORT) &= ~(1<<RS485_TX_NRX_PIN);
}



inline void watchdog_external_reset(void)
{
		vdd_pin_disable();
		vdd_pin_enable();
		if(vdd_is_disabled() == 1)
		{
			vdd_pin_disable();
		}
}

#include "rtc.h"
#include "uart.h"
#include "adc.h"
#include "extmem.h"
#include "commands.h"
#include "i2c.h"
#include "soft_pwm.h"
#include "hal_spi.h"
#include "hal_gyro_2.h"

#endif /* CONFIG_H_ */