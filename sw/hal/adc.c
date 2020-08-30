/*
 * adc.c
 *
 * Created: 27.07.2015 23:27:19
 *  Author: Andi
 */ 

#include "adc.h"
//#include <math.h>



RetVal adc_initialized(void)
{
		if(ADCSRA & (1<<ADEN))
		{
			return DONE;
		}
		
		return FAILED;
}

void adc_init(void)
 {
	 ADCSRA	= (1<<ADEN); // Enable ADC, clear all other bits
	 ADMUX = 0; // External reference voltage, right adjust
	 ADCSRA	|= (1<<ADPS2);  // Prescaler for ADC clock (CLK/16)	 > 50 kHz	 
 }
 
 void adc_deinit(void)
 {
	 ADCSRA	= 0;		 
 }
 
 RetVal adc_test(void)
 {
	 // Measure 0V, and band gap - check if in range
	 
	 uint16_t val;
	 	 
	 val = adc_read(ADC_GND_CH);
	 
	 if(val > 5)
	 {
		 return FAILED;
	 }
	 
	 val = adc_read(ADC_BANDGAP_CH);
	 //radio_printf("BG: %f\n",ADC_TO_VOLT(val));
	 
	 if((uint16_t) abs(val - ADC_FROM_VOLT(ADC_BANDGAP_V)) > ADC_FROM_VOLT(0.2))
	 {
		 return FAILED;
	 }
	 	 
	 return DONE;
 }
  
 static uint16_t adc_read_single(uint8_t adc_ch)
 {	
	ADMUX =  adc_ch & 0xFFFF;		// Set channel	
	ADCSRA |= (1<<ADSC);			// Start measurement
	while (ADCSRA & (1<<ADSC));		// Wait for measurement to finish
	
	return ADC;
 }
 
 uint16_t adc_read(uint8_t adc_ch)
 {
	uint16_t sum = 0;
	uint8_t i;
	
	if(adc_initialized() == FAILED)
	{
		adc_init();
	}
	 
	for(i=0; i<ADC_AVERAGE_SAMPLES; i++)
	{
		sum += adc_read_single(adc_ch);	 
	}
	
	return (uint16_t) (sum / ADC_AVERAGE_SAMPLES);
 }
