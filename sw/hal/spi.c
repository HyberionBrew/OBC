/*
* spi.c
*
* Created: 02.06.2017
*  Author: asinn
*/

#include "spi.h"

static void spi_transfer_start(void);
static void spi_transfer_start_polling(void);


inline void cs_gyro2_select(void)
{
	PORT(CS_GYRO2_PORT) &= ~(1<<CS_GYRO2_PIN);
}

inline void cs_gyro2_unselect(void)
{
	PORT(CS_GYRO2_PORT) |= (1<<CS_GYRO2_PIN);
}

inline void cs_mag2_select(void)
{
	PORT(CS_MAG2_PORT) &= ~(1<<CS_MAG2_PIN);
}

inline void cs_mag2_unselect(void)
{
	PORT(CS_MAG2_PORT) |= (1<<CS_MAG2_PIN);
}

void spi_init(void)
{
	// Init chip select lines of attached device
	DDR(CS_FLASH_PORT) |= (1<<CS_FLASH_PIN);
	DDR(CS_GYRO2_PORT) |= (1<<CS_GYRO2_PIN);
	DDR(CS_MAG2_PORT) |= (1<<CS_MAG2_PIN);		// Slave select pin of SPI needs to be defined as output for master mode!
	
	// Unselect all device
	cs_gyro2_unselect();
	cs_mag2_unselect();
	
	// Init SPI pins
	DDR(SPI_PORT) |= (1<<SPI_MISO_PIN);
	DDR(SPI_PORT) |= (1<<SPI_MOSI_PIN);
	DDR(SPI_PORT) |= (1<<SPI_SCK_PIN);
	
	// SPI control register
	SPCR  = (1<<SPIE)  |(1<<SPE) | (1<<MSTR) | (1<<SPR0);	// Enable SPI Interrupt, enable SPI, MSB first, Master Mode, CPOL = 0, CPHA = 0, Clock rate divider = CLK/4
	
	// SPI status register
	SPSR = (1<<SPI2X);	// Enable double clock rate: CLK/2
	
	// Clear SPI data register
	uint8_t dummy = SPDR;
	(void) dummy;
	
	// clear SPIF bit in SPSR
	sei();
}

#define SPI_RX_SIZE 8

typedef struct spi_job_s
{
	uint8_t nb; // Total number of bytes in transfer
	uint8_t cb;	// Current byte position
	
	uint8_t tx[SPI_RX_SIZE+1];
	uint8_t rx[SPI_RX_SIZE];

	uint8_t active;
	uint8_t done;

} volatile spi_job_t;

spi_job_t spi_job;


typedef enum
{
	SPIR_IDLE = 0,
	SPIR_GYRO1,
	SPIR_MAX
} spi_routine;

#if SPIR_MAX == 8
#error "Too many SPI routines defined"
#endif

#define SPI_TX_DUMMY 0x00

#define SPI_READ(reg) (0x80 | (reg))
#define GYRO1_AUTO_INC(reg) (0x40 | (reg))
/*void gyro1_init(void) // A3G4250D
{
	// --- Read device ID ---
	spi_job.tx[0] = SPI_READ(0x0F);	// Read WHOAMI register
	spi_job.rx[1] = 0x00;
	spi_job.nb = 2;						// 2 Byte transfer

	// Start transfer
	cs_gyro1_select();
	spi_transfer_start_polling();
	cs_gyro1_unselect();
	
	
	
	if(spi_job.rx[1] != 0xD3)
	{
		// Wrong ID - communication failed
		return;
	}
		
	// --- Set control register 1 ---
	spi_job.tx[0] = 0x20;	// Write to ctr reg 1
	// 12.5 Hz bandwidth, 100 Hz ODR, X,Y,Z enabled, PD disabled
	spi_job.tx[1] = 0x0F; // (0<<DR1) | (0<<DR0) | (0<<BW1) | (0<<BW0) | (1<<PD) |(1<<ZEN) |(1<<YEN) |(1<<XEN);
	spi_job.nb = 2;						// 2 Byte transfer

	cs_gyro1_select();
	spi_transfer_start_polling();
	gyro2_init();
	cs_gyro1_unselect();
	radio_printf("Gyro 1 initialized\n");
}*/

void gyro2_init(void) // BMG250
{
	// --- Read device ID ---
	spi_job.tx[0] = SPI_READ(0x00);	// Read chip ID register
	spi_job.rx[1] = 0x00;
	spi_job.nb = 2;						// 2 Byte transfer

	// Start transfer
	cs_gyro2_select();
	spi_transfer_start_polling();
	cs_gyro2_unselect();
	
	if(spi_job.rx[1] != 0xD5)
	{
		// Wrong ID - communication failed
		return;
	}
	
	// --- Set CMD register ---
	spi_job.tx[0] = 0x7E;	// Write to CMD
	// gyr_norm ... Set PMU to normal mode
	spi_job.tx[1] = 0x15; //
	spi_job.nb = 2;						// 2 Byte transfer

	cs_gyro2_select();
	spi_transfer_start_polling();
	cs_gyro2_unselect();
	
	// --- Set GYR_CONF register ---
	spi_job.tx[0] = 0x42;	// Write to GYR_CONF
		//TODO! thats not right imo
	// 10.7 Hz bandwidth, 25 Hz ODR, 4x oversampling, X,Y,Z enabled, PD disabled

	spi_job.tx[1] = 0x06; // (0<<DR1) | (0<<DR0) | (0<<BW1) | (0<<BW0) | (1<<PD) |(1<<ZEN) |(1<<YEN) |(1<<XEN);
	spi_job.nb = 2;						// 2 Byte transfer

	cs_gyro2_select();
	spi_transfer_start_polling();
	cs_gyro2_unselect();
	
	// --- Set GYR_RANGE register ---
	spi_job.tx[0] = 0x43;	// Write to GYR_RANGE
	// +-500°/s; 15.3m°/s / LSB
	spi_job.tx[1] = 0x02; // 
	spi_job.nb = 2;						// 2 Byte transfer

	cs_gyro2_select();
	spi_transfer_start_polling();
	cs_gyro2_unselect();	
	
	radio_printf("Gyro 2 initialized\n");
}

void mag2_init(void) // AK09912C
{
	// --- Read device ID ---
	spi_job.tx[0] = SPI_READ(0x01);	// Read DEVIC ID register
	spi_job.rx[1] = 0x00;
	spi_job.nb = 2;						// 2 Byte transfer

	// Start transfer
	cs_mag2_select();
	spi_transfer_start_polling();
	cs_mag2_unselect();
	
	radio_printf("Mag2: %x \n", (spi_job.rx[1]));
	
	if(spi_job.rx[1] != 0x04)
	{
		// Wrong ID - communication failed
		return;
	}
	
	// --- Set control register 1 ---
	/*spi_job.tx[0] = 0x20;	// Write to ctr reg 1
	// 12.5 Hz bandwidth, 100 Hz ODR, X,Y,Z enabled, PD disabled
	spi_job.tx[1] = 0x0F; // (0<<DR1) | (0<<DR0) | (0<<BW1) | (0<<BW0) | (1<<PD) |(1<<ZEN) |(1<<YEN) |(1<<XEN);
	spi_job.nb = 2;						// 2 Byte transfer

	cs_gyro1_select();
	spi_transfer_start_polling();
	cs_gyro1_unselect();
	*/
	radio_printf("Mag 2 initialized\n");
}




#define GYRO1_TEMPERATURE_OFFSET 11
#define GYRO1_TEMPERATURE_TO_DEG(INT) ((GYRO1_TEMPERATURE_OFFSET + (INT))) 

#define GYRO2_TEMPERATURE_OFFSET 23
#define GYRO2_TEMPERATURE_TO_DEG(INT) (GYRO2_TEMPERATURE_OFFSET + (INT))
/*
void gyro1_read_temperature(void)
{
	// --- Read temperature ---
	spi_job.tx[0] = SPI_READ(0x26);	// Read temperature register
	spi_job.nb = 2;						// 2 Byte transfer

	// Start transfer
	cs_gyro1_select();
	spi_transfer_start_polling();
	cs_gyro1_unselect();
	
	radio_printf("Gyro1: %d C \n",GYRO1_TEMPERATURE_TO_DEG(spi_job.rx[1]));
}
*/
void gyro2_read_temperature(void)
{
	// --- Read temperature ---
	spi_job.tx[0] = SPI_READ(0x21);	// Read temperature register
	spi_job.nb = 2;						// 2 Byte transfer, skip lsb

	// Start transfer
	cs_gyro2_select();
	spi_transfer_start_polling();
	cs_gyro2_unselect();
	
	radio_printf("Gyro2: %d C \n",GYRO2_TEMPERATURE_TO_DEG(spi_job.rx[1]));
}
/*
void gyro1_read_values(void)
{
	// --- Read temperature ---
	spi_job.tx[0] = GYRO1_AUTO_INC(SPI_READ(0x28));	// Read data registers register
	spi_job.nb = 7;						// 7 Byte transfer

	// Start transfer
	cs_gyro1_select();
	spi_transfer_start_polling();
	cs_gyro1_unselect();
	
	radio_printf("Gyro1 X: %d  \n",(spi_job.rx[2]<<8) | spi_job.rx[1]);
	radio_printf("Gyro1 Y: %d  \n",(spi_job.rx[4]<<8) | spi_job.rx[3]);
	radio_printf("Gyro1 Z: %d  \n",(spi_job.rx[6]<<8) | spi_job.rx[5]);
}
*/
void gyro2_read_values(void)
{
	// --- Read temperature ---
	spi_job.tx[0] = (SPI_READ(0x12));	// Read data registers register
	spi_job.nb = 7;						// 7 Byte transfer

	// Start transfer
	cs_gyro2_select();
	spi_transfer_start_polling();
	cs_gyro2_unselect();
	
	radio_printf("Gyro2 X: %d  \n",(spi_job.rx[2]<<8) | spi_job.rx[1]);
	radio_printf("Gyro2 Y: %d  \n",(spi_job.rx[4]<<8) | spi_job.rx[3]);
	radio_printf("Gyro2 Z: %d  \n",(spi_job.rx[6]<<8) | spi_job.rx[5]);
}


// Sending Routine Master Mode (polling)
void spi_process(spi_routine new_routine)
{
	static spi_routine current_routine = SPIR_IDLE;
	static uint8_t routines = 0;
	
	return;
	if(new_routine != SPIR_IDLE)
	{
		if(new_routine >= 8)
		{
			return;
		}
		
		routines |= (1<<new_routine);
	}
	
	
	switch(current_routine)
	{
		case SPIR_GYRO1:
		if(spi_job.done == 0)
		{
			spi_job.tx[0] = 0x11;
			spi_job.tx[1] = 0x22;
			spi_job.nb = 2;
			cs_gyro2_select();
			spi_transfer_start();
		}
		else
		{
			// Jobe done - release chip select
			cs_gyro2_unselect();
			current_routine = SPIR_IDLE; // temporary only!
			
		}


		
		
		break;
		
		case SPIR_IDLE: // Idle - do nothing
		spi_transfer_start_polling();
		break;
		
		default:
		break;
	}
	
}

static void spi_transfer_start(void)
{
	spi_job.done = 0;
	spi_job.cb = 1;
	spi_job.nb--;
	SPDR = spi_job.tx[0];
}

static void spi_transfer_start_polling(void)
{
	spi_transfer_start();
	
	// Wait for completion of SPI job
	while(spi_job.done == 0);
}

// Interrupt Routine Master Mode (interrupt controlled)
ISR(SPI_STC_vect)
{
	// Read received byte
	spi_job.rx[spi_job.cb-1] = SPDR;
	
	if(spi_job.nb-- == 0)
	{
		spi_job.done = 1;
		spi_job.active = 0;
		return;
	}

	// Transmit next data or dummy byte
	SPDR = spi_job.tx[spi_job.cb++];
}

