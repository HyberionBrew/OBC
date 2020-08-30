/*
 * xmem.c
 *
 * Created: 17.05.2017 09:47:26
 *  Author: asinn
 */ 

#include "extmem.h"

#define FRAM_SIZE 59000 // Bytes

void fram_select_program_memory(void);
void fram_select_data_memory(void);

// Linker options: -lprintf_flt -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80FB60

typedef struct obc_nv_data_s
{
	uint32_t reset_counter;
	uint32_t ontime; // in seconds
} obc_nv_data_t;

static obc_nv_data_t *obc_nv_data;

void func(void);
void nv_struct_init(void);

void fram_init(void)
{
	// Configure adress block selection pin
	DDR(A16_PORT) |= (1<<A16_PIN);
	fram_select_program_memory();
	
	// External memory interface enable
	MCUCR |= (1<<SRE);// |(1<<SRW10); // Wait state
	
	// Sector limit - one sector only, two cycle wait states
	XMCRA = 0;//(1<<SRW01); //(1<<SRW11) | (1<<SRW00) | 
	
	/*
	MCUCR |= (1<<SRE) |(1<<SRW10); // Wait state
	
	// Sector limit - on sector only, no wait states
	XMCRA =  (1<<SRW01)| (1<<SRW00);*/

	// Bus keeper enabled, use all 60 kByte, 
	XMCRB = 0;// (1<<XMBK); // Use 60kbyte of external memory
	
	//nv_struct_init();
	//func();
}

void fram_deinit(void)
{
	MCUCR &= ~(1<<SRE);
}

inline void fram_select_program_memory(void)
{
	PORT(A16_PORT) |= (1<<A16_PIN);
}

inline void fram_select_data_memory(void)
{
	PORT(A16_PORT) &= ~(1<<A16_PIN);
}

void nv_struct_init(void)
{
	 obc_nv_data = (obc_nv_data_t*) malloc(sizeof(obc_nv_data_t));
	 radio_printf("PRT: %p \n",obc_nv_data);
	 // Todo: Check if valid
}

void func(void)
{
	  fram_select_data_memory();	 
	  obc_nv_data->reset_counter++;

	  radio_printf("Reset counter: %d\n",obc_nv_data->reset_counter);
}

void ontime_inc(uint8_t duration)
{
	 fram_select_data_memory();
	 obc_nv_data->ontime += duration; // in seconds
	 radio_printf("Ontime: %d\n",obc_nv_data->ontime);
}

