/*
 * omnic_basic.c
 *
 * Created: 22.11.2016 15:20:27
 *  Author: asinn
 */ 


// neu
#include "config.h"

bool vdd_disabled;




void mt_init(void)
{
	DDR(MT_ENABLE_PORT) |= (1<<MT_ENABLE_PIN);
	mt_disable();
}

void vdd_disable(void)
{
	vdd_pin_disable();
	vdd_disabled = 1;
}

void vdd_enable(void)
{
	vdd_pin_enable();
	vdd_disabled = 0;
}

bool mt_enable(void)
{
	PORT(MT_ENABLE_PORT) |= (1<<MT_ENABLE_PIN);
	return (mt_state() == 1);
}

bool mt_disable(void)
{
	PORT(MT_ENABLE_PORT) &= ~(1<<MT_ENABLE_PIN);
	return (mt_state() == 0);
}

bool mt_state(void)
{
	// Return 1 if MT is enabled
	return ((PIN(MT_ENABLE_PORT) & (1<<MT_ENABLE_PIN)));
}


void rs485_tx_nrx_init(void)
{
	DDR(RS485_TX_NRX_PORT) |= (1<<RS485_TX_NRX_PIN);
	rs485_tx_nrx_enable();
}





void vdd_init(void)
{
	DDR(VDD_DISABLE_PORT) |= (1<<VDD_DISABLE_PIN);
	vdd_enable();
}


void rbf_init(void)
{
	// Set RBF pin to input, no pullup
	DDR(RBF_PORT) &= ~(1<<RBF_PIN);
}

rbf_state rbf_inserted(void)
{
	// Return INSERTED if RBF is inserted
	return !((PIN(RBF_PORT) & (1<<RBF_PIN)));
}

#if 0

#include <avr/power.h>


status_t status;
callback_t minute_callback;
callback_t watchdog_callback;
callback_t hourly_callback;

void omnic_init(void)
{
	// --- Clock ---
	CLKPR = CPU_CLOCK_PRESCALER_POWER & 0x0F;
	//OSCCAL = 70;
	
	// Deactivate watchdog
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	CCP = 0xD8; // Write signature 0xD8 to allow to disable watchdog
	/* Turn off watchdog */
	WDTCSR = 0x00;
	
	// Init GPIO related hardware
	led_internal_init();
	dpo1_init();
	dpo2_init();
	din1_init();
	
	// Initialize EEPROM and increment reset counter
	ep_init();
	
	
	#if ADC_USED
		adc_init();
	#else
		power_adc_disable();
	#endif
	
	// Enable watchdog interrupt
	WDTCSR = (1<<WDIE) | (1<<WDP3); // 4s interrupt -> Timeout has to be updated in WATCHDOG_TIMEOUT!
 
	
	watchdog_callback = NULL;
	hourly_callback = NULL;
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);	
}

void set_hourly_callback(callback_t callback)
{
	hourly_callback = callback;	
}

void set_watchdog_callback(callback_t callback)
{
	watchdog_callback = callback;
}

void set_minute_callback(callback_t callback)
{
	minute_callback = callback;
}

ISR(WDT_vect)
{
	sleep_disable();
	cli();
	wdt_reset(); // Reset watchdog counter
	led_internal_on();
	_delay_us(100);
	led_internal_off();

	
	
	

	
	// Minute timeout
	if((status.timestamp % 60) == 0)
	{
		// --- Minute timeout ---------------------------------------------
		if(minute_callback != NULL)
		{
			minute_callback();
		}
		
		#if TEMPERATURE_MEASUREMENT_USED 
		#if !ADC_USED		
		adc_init();			
		#endif
		adc_read_average(TEMPARTURE_INTERNAL_ADC_CH);
		#if !ADC_USED
		adc_deinit();
		#endif
		#endif
		
		
		// --- End minute timeout ------------------------------------------		
		if((status.timestamp % 3600) == 0)
		{
			// --- Hourly timeout ----------------------------------------------
			ep_ontime_update(3600);
			
			if(hourly_callback != NULL)
			{
				hourly_callback();
			}
			
			
			
			
			
			
			// --- End hourly timeout -------------------------------------------
		}		
	}
	
	

	status.timestamp += WATCHDOG_TIMEOUT;
	sei();
}

void update_internal_temperature(void)
{
	status.temperature_internal = (ADC_RES * TEMPERATURE_INTERNAL_SLOPE *  ((float) adc_read_average(TEMPARTURE_INTERNAL_ADC_CH)) - TEMPERATURE_INTERNAL_OFFSET);
}



// --- Internal LED ---
void led_internal_init(void)
{
		DDR(LED_INTERNAL_PORT) |= (1<<LED_INTERNAL_PIN);
		led_internal_off();	
}

void led_internal_on(void)
{
	PORT(LED_INTERNAL_PORT) |= (1<<LED_INTERNAL_PIN);
}

void led_internal_off(void)
{
	PORT(LED_INTERNAL_PORT) &= ~(1<<LED_INTERNAL_PIN);
}

bool led_internal_state(void)
{
	return ((PIN(LED_INTERNAL_PORT) & (1<<LED_INTERNAL_PIN)));
}

void led_internal_toggle(void)
{
	if(led_internal_state())
	{
		led_internal_off();
	}
	else
	{
		led_internal_on();
	}
}

void led_internal_blink(uint8_t times)
{
	for(;times>0;times--)
	{
		led_internal_on();
		_delay_ms(10);
		led_internal_off();
		_delay_ms(50);
	}
}




void dpo1_init(void)
{
	DDR(DPO1_PORT) |= (1<<DPO1_PIN);
	dpo1_off();
	
	#if DPO1_PWM_OUTPUT
	
	#endif
}

void dpo1_on(void)
{
	PORT(DPO1_PORT) |= (1<<DPO1_PIN);
}

void dpo1_off(void)
{
	PORT(DPO1_PORT) &= ~(1<<DPO1_PIN);
}

bool dpo1_state(void)
{
	return ((PIN(DPO1_PORT) & (1<<DPO1_PIN)));
}


void dpo1_toggle(void)
{
	if(dpo1_state())
	{
		dpo1_off();
	}
	else
	{
		dpo1_on();
	}
}

void dpo2_init(void)
{
	DDR(DPO2_PORT) |= (1<<DPO2_PIN);
	dpo2_off();
	
		#if DPO2_PWM_OUTPUT
		
		#endif
}

void dpo2_on(void)
{
	PORT(DPO2_PORT) |= (1<<DPO2_PIN);
}

void dpo2_off(void)
{
	PORT(DPO2_PORT) &= ~(1<<DPO2_PIN);
}

bool dpo2_state(void)
{
	return ((PIN(DPO2_PORT) & (1<<DPO2_PIN)));
}


void dpo2_toggle(void)
{
	if(dpo2_state())
	{
		dpo2_off();
	}
	else
	{
		dpo2_on();
	}
}

// --- Inputs ---
// Digital inputs

void din1_init(void)
{
		DDR(DIN1_PORT) &= ~(1<<DIN1_PIN);
		din1_pullup_en(0);
}

void din1_pullup_en(bool enable)
{
	if(enable)
		PORT(DIN1_PORT) |= (1<<DIN1_PIN);
	else
		PORT(DIN1_PORT) &= ~(1<<DIN1_PIN);
}

bool din1_state(void)
{
	return ((PIN(DIN1_PORT) & (1<<DIN1_PIN)));
}

#endif