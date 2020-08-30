/**
* \file
*
* \brief Application to generate sample driver to AVRs TWI module
*
* Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
*
* \asf_license_start
*
* \page License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel micro controller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"

RetVal i2c_transceive_polling(void);

typedef struct i2c_job_s
{
	uint8_t adr;
	uint8_t tbuf[2];
	uint8_t rbuf[TWI_BUFFER_SIZE];	// Rx buf
	uint8_t rc;	// Bytes to receive
	uint8_t tc; // Bytes to send
	uint8_t p; // Bytes sent/received

	uint8_t rxtx_active :1;
	uint8_t error :1;
	uint8_t unused :6;

} volatile i2c_job_t;

static i2c_job_t i2c_job;

void mag1_read_temperature(void)
{
	i2c_job.adr = 0x0E;		// Set slave address
	i2c_job.tbuf[0] = 0x0F;	// Register
	i2c_job.tc = 1;
	i2c_job.rc = 1;			// Number of bytes to read

	i2c_transceive_polling();
	
	radio_printf("Mag1: %d C\n",((int8_t) i2c_job.rbuf[0])+25);	
}

#define MAG1_INT2FLOAT(val) ((val)*0.1)


void mag1_read_values(void)
{
	i2c_job.adr = 0x0E;		// Set slave address
	i2c_job.tbuf[0] = 0x01;	// Data register
	i2c_job.tc = 1;
	i2c_job.rc = 7;			// Number of bytes to read

	i2c_transceive_polling();
	
	int16_t x = (i2c_job.rbuf[0]<<8) | i2c_job.rbuf[1];
	int16_t y = (i2c_job.rbuf[2]<<8) | i2c_job.rbuf[3];
	int16_t z = (i2c_job.rbuf[4]<<8) | i2c_job.rbuf[5];
	
	if(i2c_job.rbuf[6] != 0xC4)
	{
		radio_printf("Mag1 error\n");
	}

	radio_printf("Mag1: %.1f, %.1f, %.1f uT\n",MAG1_INT2FLOAT(x),MAG1_INT2FLOAT(y),MAG1_INT2FLOAT(z));
}

void mag1_set_offset(uint16_t x, uint16_t y, uint16_t z)
{	
	i2c_job.adr = 0x0E;		// Set slave address
	i2c_job.tbuf[0] = 0x09;			// OFF_X_MSB reg adr
	i2c_job.tbuf[1] = (x >> 8);		// OFF_X_MSB
	i2c_job.tbuf[2] = (x & 0xFF);	// OFF_Y_MSB
	i2c_job.tbuf[3] = (y >> 8);		// OFF_Y_LSB
	i2c_job.tbuf[4] = (y & 0xFF);	// OFF_X_MSB
	i2c_job.tbuf[5] = (z >> 8);		// OFF_Z_MSB
	i2c_job.tbuf[6] = (z & 0xFF);	// OFF_Z_LSB
	
	i2c_job.tc = 7;			// Number of bytes to send
	i2c_job.rc = 0;			// Number of bytes to read

	i2c_transceive_polling();	
}

void mag1_init(void)
{
	i2c_job.adr = 0x0E;		// Set slave address
	i2c_job.tbuf[0] = 0x07;		// Register
	i2c_job.tc = 1;

	i2c_job.rc = 1;			// Number of bytes to read
	i2c_job.rbuf[0] = 0;	

	i2c_transceive_polling();
		
	if(i2c_job.rbuf[0] != 0xC4)	
	{
		// Wrong ID received
		return;
	}
		
	i2c_job.adr = 0x0E;		// Set slave address
	i2c_job.tbuf[0] = 0x10;		// CTRL_REG1 
	i2c_job.tbuf[1] = ((1<<5) | (1<<4) | (1<<0));		// CTRL_REG1, 10 Hz output, 640 Hz sampling: DR0 = 1, OS1 = 1
	i2c_job.tbuf[2] = ((1<<7));		// CTRL_REG2, Auto_mrst_en = 1
	i2c_job.tc = 3;			// Number of bytes to send

	i2c_job.rc = 0;			// Number of bytes to read

	i2c_transceive_polling();	
	
	// --- Check current system mode ---
	i2c_job.adr = 0x0E;		// Set slave address
	i2c_job.tbuf[0] = 0x08;		// SYSMOD register
	i2c_job.tc = 1;

	i2c_job.rc = 1;			// Number of bytes to read
	i2c_job.rbuf[0] = 0;

	i2c_transceive_polling();
	
	if((i2c_job.rbuf[0] & 0x03) != 0x02)
	{
		// Sensor is not in normal mode
		return;
	}	
			
	radio_printf("Mag1 initialized.\n");
}

void i2c_init(void)
{
	TWCR = 0;                                 // Enable TWI-interface and release TWI pins.
	TWBR = TWI_TWBR;                                  // Set bit rate register (Baud rate). Defined in header file.Driver presumes prescaler to be 00.
	TWDR = 0xFF;                                      // Default content = SDA released.
	TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins.
	(0<<TWIE)|(0<<TWINT)|                      // Disable Interrupt.
	(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests.
	(0<<TWWC);                                 //
	
	i2c_job.rxtx_active = 0;
	i2c_job.error = 0;
}

inline unsigned char i2c_busy( void )
{
	return ( TWCR & (1<<TWIE) );                  // IF TWI Interrupt is enabled then the Transceiver is busy
}

void i2c_transceive(void)
{
	if ( i2c_busy() || i2c_job.rxtx_active)
	{
		// I2C is busy
		return;
	}
	
	if(i2c_job.tc == 0)
	{
		return;
	}
	
	// Shift address by W/R Bit
	i2c_job.adr = (i2c_job.adr<<TWI_ADR_BITS);	
	
	// Reset job variables
	i2c_job.rxtx_active = 1;
	i2c_job.error = 0;
	i2c_job.p = 0;
	
	TWCR = (1<<TWEN)|                             // TWI Interface enabled.
	(1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interrupt and clear the flag.
	(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
	(0<<TWWC);                             //
}

RetVal i2c_transceive_polling(void)
{
	    i2c_transceive();      
		while(i2c_job.rxtx_active == 1);   
		
		return i2c_job.error;         
}

#define TWI_REG_SENT 0xFF

ISR(TWI_vect)
{
	//static unsigned char TWI_bufPtr;
	
	switch (TWSR)
	{
		case TWI_START:             // START has been transmitted
		case TWI_REP_START:         // Repeated START has been transmitted
		// Transmit address
		//radio_printf("start cond");
		TWDR = i2c_job.adr;
		TWCR = (1<<TWEN)|                                 // TWI Interface enabled
		(1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interrupt and clear the flag to send byte
		(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //
		(0<<TWWC);
		break;
		
		case TWI_MTX_ADR_ACK:       // SLA+W has been transmitted and ACK received
		case TWI_MTX_DATA_ACK:      // Data byte has been transmitted and ACK received
		//radio_printf("slaw + ack\n");
		if (i2c_job.p < i2c_job.tc)
		{
			//radio_printf("send reg\n");
			TWDR = i2c_job.tbuf[i2c_job.p++];
			TWCR = (1<<TWEN)|                                 // TWI Interface enabled
			(1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interrupt and clear the flag to send byte
			(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //
			(0<<TWWC);                                 //
		}
		else                    // Send STOP after last byte
		{
			if (i2c_job.rc > 0)
			{
				i2c_job.p = 0;
				//radio_printf("start rx\n");
				// Read data from slave - generate repeated start condition				
				i2c_job.adr |= (TRUE<<TWI_READ_BIT); // Set read bit
				
				TWCR = (1<<TWEN)|                      // TWI Interface enabled.
				(1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interrupt and clear the flag.
				(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
				(0<<TWWC);                             //
			}
			else
			{
				//radio_printf("done\n");
				// Everything done
				TWCR = (1<<TWEN)|                                 // TWI Interface enabled
				(0<<TWIE)|(1<<TWINT)|                      // Disable TWI Interrupt and clear the flag
				(0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.
				(0<<TWWC);                                 //
				i2c_job.rxtx_active = 0;
			}
		}
		break;
		case TWI_MRX_DATA_ACK:      // Data byte has been received and ACK transmitted
		i2c_job.rbuf[i2c_job.p++] = TWDR;
		
		case TWI_MRX_ADR_ACK:       // SLA+R has been transmitted and ACK received
		//radio_printf("slar + ack\n");
		if (i2c_job.p < (i2c_job.rc-1))                  // Detect the last byte to NACK it.
		{
			
			TWCR = (1<<TWEN)|                                 // TWI Interface enabled
			(1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interrupt and clear the flag to read next byte
			(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Send ACK after reception
			(0<<TWWC);                                 //
		}
		else                    // Send NACK after next reception
		{//
			//radio_printf("nack next\n");
			TWCR = (1<<TWEN)|                                 // TWI Interface enabled
			(1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interrupt and clear the flag to read next byte
			(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Send NACK after reception
			(0<<TWWC);                                 //
		}
		break;
		case TWI_MRX_DATA_NACK:     // Data byte has been received and NACK transmitted
		//radio_printf("Nack sent\n");
		i2c_job.rbuf[i2c_job.p] = TWDR;
		
		TWCR = (1<<TWEN)|                                 // TWI Interface enabled
		(0<<TWIE)|(1<<TWINT)|                      // Disable TWI Interrupt and clear the flag
		(0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.
		(0<<TWWC);                                 //
		i2c_job.rxtx_active = 0;                 // Set status bits to completed successfully.
		//radio_printf("rx done\n");
		break;
		case TWI_ARB_LOST:          // Arbitration lost
		
		TWCR = (1<<TWEN)|                                 // TWI Interface enabled
		(1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interrupt and clear the flag
		(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|           // Initiate a (RE)START condition.
		(0<<TWWC);                                 //
		i2c_job.error = 1;
		i2c_job.rxtx_active = 0;
		
		break;
		case TWI_MTX_ADR_NACK:      // SLA+W has been transmitted and NACK received
		case TWI_MRX_ADR_NACK:      // SLA+R has been transmitted and NACK received
		case TWI_MTX_DATA_NACK:     // Data byte has been transmitted and NACK received
		case TWI_BUS_ERROR:         // Bus error due to an illegal START or STOP condition
		default:
		i2c_job.rbuf[0] = TWSR;                                 // Store TWSR and automatically sets clears noErrors bit.
		// Reset TWI Interface
		TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins
		(0<<TWIE)|(0<<TWINT)|                      // Disable Interrupt
		(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests
		(0<<TWWC);                                 //
		i2c_job.error = 1;
		i2c_job.rxtx_active = 0;
	}
}
