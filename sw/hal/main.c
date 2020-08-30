/*
* DISCO_OBC_Firmware.c
*
* Created: 13.04.2017 14:52:09
* Author : asinn
*/

#include "config.h"

void sbs_interrupt(void);
#include <avr/power.h>

int main(void)
{
	// --- Clock ---
	clock_prescale_set(clock_div_8);
		
	vdd_init();
	vee_init();
	_delay_ms(200);
	vdd_enable();
	vee_enable();
	hal_spi_init();
	hal_spi_gyro_2_init();
	
	while(1)
	{
	}
	
	
	mt_init();
	
	rtc_init();		// Initialize RTC module for UTC time (1 second interrupt period)
	set_second_callback(&sbs_interrupt);	// Set function called by second by second interrupt
	
	uart_radio_init();
	
	
	i2c_init();
	
	sei();	// Enable global interrupts
	
	radio_printf("\n--- OBC-INFO ---\n");
	radio_printf("HW-VER: %s\n","EM1.0");
	radio_printf("Commit: %s\n",GIT_COMMIT);

	adc_init();
	if(adc_test() == FAILED)
	{
		radio_printf("ADC test failed.\n");
	}
	else
	{
		radio_printf("ADC test successful.\n");
	}	

	char rx[32];
	uint8_t rxl;

	// mt_init();
		
	while (1)
	{
		_delay_ms(1000);
		cmd_execute_next();
				
		radio_printf("T%d: ",rtc_get_time());
		radio_printf("TMP36: %.1f C\n",  TMP36_FROM_VOLT(ADC_TO_VOLT(adc_read(TEMPERATURE_ADC_CH))));
		radio_printf("%f V\n",ADC_TO_VOLT(adc_read(SUPPLY_CURRENT_ADC_CH)));
		radio_printf("%.3f V\n",(ADC_TO_VOLT(adc_read(SUPPLY_CURRENT_ADC_CH))));
		radio_printf("%.3f mA\n",SUPPLY_CURRENT_FROM_VOLT(ADC_TO_VOLT(adc_read(SUPPLY_CURRENT_ADC_CH))));
		
		rxl = radio_receive(rx,32);
		if(rxl > 0)
		{
			radio_printf("RX: ");
			radio_transmit(rx,rxl);
			received2cmd(rx);
		}
		
	}
}

void sbs_interrupt(void)
{
	// Second by second interrupt: Called every second by RTC
	static uint8_t counter = 0;
	
	watchdog_external_reset();
	
	if((counter%10) == 0)
	{
		
	}
	


	counter++;
}

