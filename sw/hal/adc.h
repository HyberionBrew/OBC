#ifndef ADC_H
#define ADC_H
/*
 * adc.h
 *
 * Created: 27.07.2015 15:56:50
 *  Author: Andi
 */ 

#include "config.h"

#define ADC_BANDGAP_CH	0x1E
#define ADC_GND_CH	   	0x1F

#define ADC_BANDGAP_V	1.24 // V

#define ADC_VREF 2.50 // V
#define ADC_RES ((ADC_VREF) / 1024.0)
#define ADC_AVERAGE_SAMPLES		4

#define ADC_FROM_VOLT(V) ((uint16_t) ((V) / (ADC_RES))) // Is computed at compile time
#define ADC_TO_VOLT(INT) ((float) ((INT) * (ADC_RES))) // Is computed by CPU

#define TMP36_SENS		((float) 0.01)	// mV/°C
#define TMP36_OFFSET	((float) 0.5)		//  V

#define TMP36_FROM_VOLT(v)		((float) ((v) - (TMP36_OFFSET)) / (TMP36_SENS))
#define TMP36_TO_VOLT(celsius)	((float) ((celsius) * (TMP36_SENS)) + (TMP36_OFFSET))

#define SUPPLY_CURRENT_SENSE_RESISTOR 0.05 // Ohm
#define SUPPLY_CURRENT_GAIN	100

#define SUPPLY_CURRENT_FROM_VOLT(v)	((float) ((v) * (1000 / (SUPPLY_CURRENT_SENSE_RESISTOR * SUPPLY_CURRENT_GAIN)))) // in mA


#if ADC_AVERAGE_SAMPLES > (65536/1024)
#error "Number of samples for ADC averaging too high!"
#elif ADC_AVERAGE_SAMPLES <= 0
#error "Number of samples for ADC averaging invalid!"
#endif

#if F_CPU / 16 < 50000
#warning "ADC clock is smaller than 50kHz!"
#endif

void adc_init(void);
void adc_deinit(void);
RetVal adc_test(void);
uint16_t adc_read(uint8_t adc_ch);

#endif	// ADC_H