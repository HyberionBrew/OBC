/**
* @file hal_spi.c
* @author Fabian Kresse
* @date 17 November 2019
* @brief The SPI HAL interface for the ATMEGA1280L
*
* @details interface for communicating with Flash(TC58CVG2S0), Gyro 2 (BMG250) and Mag 2 (FXOS8700).
* All of them are in the region VDD.
* for details: @see https://data.spaceteam.at/index.php/s/a4C2sJonWyqJgKn
*/

#include "hal_spi.h"
#include <avr/io.h>

#define READ_REG(reg) (0x80|reg)
#define WRITE_REG(reg) (0x00|reg)
#define SPI_DUMMY 0xF0

static inline void cs_gyro2_select(void)
{
	PORT(CS_GYRO2_PORT) &= ~(1<<CS_GYRO2_PIN);
}

static inline void cs_gyro2_unselect(void)
{
	PORT(CS_GYRO2_PORT) |= (1<<CS_GYRO2_PIN);
}

static inline void cs_mag2_select(void)
{
	PORT(CS_MAG2_PORT) &= ~(1<<CS_MAG2_PIN);
}

static inline void cs_mag2_unselect(void)
{
	PORT(CS_MAG2_PORT) |= (1<<CS_MAG2_PIN);
}


/**
 * @brief initializes the SPI
 * @details 
 *	SPCR (SPI Control register) settings:
 *		enable SPI Interrupt, enable SPI, MSB first, 
 *		Master Mode, CPOL = 0, CPHA = 0, Clock rate divider = CLK/4
 *
 *	after call sei() should be enabled (currently gets set here! TODO!)
 * @returns void
 */
void hal_spi_init(void){
	//Power reduction register needs to be set to 0
	PRR0 &= ~(1 << PRSPI);
	//Init chip select lines of attached devices.
	DDR(CS_FLASH_PORT) |= (1<<CS_FLASH_PIN);
	DDR(CS_GYRO2_PORT) |= (1<<CS_GYRO2_PIN);
	DDR(CS_MAG2_PORT) |= (1<<CS_MAG2_PIN);	
	
	//Unselect all devices.
	cs_gyro2_unselect();
	cs_mag2_unselect();
	//eeprom TODO!
	
	DDR(SPI_PORT) |= (1<<SPI_MISO_PIN);
	DDR(SPI_PORT) |= (1<<SPI_MOSI_PIN);
	DDR(SPI_PORT) |= (1<<SPI_SCK_PIN);
	
	//! Enable SPI Interrupt, enable SPI, MSB first, Master Mode, CPOL = 0, CPHA = 0, Clock rate divider = CLK/4
	SPCR  = (1<<SPE) | (1<<MSTR) | (1<<SPR0);	

	// SPI status register
	SPSR = (1<<SPI2X);	// Enable double clock rate: CLK/2
	
	//uint8_t dummy = SPDR;
}
/**
 * @brief writes to the specified register <b>reg<b/> the value <b>val</b>.
 * @param reg the register the value should be written to.
 * @param val the value that should be written to the register.
 *
 * @returns true on success. 
 */
bool spi_write_reg(uint8_t reg, uint8_t val){
	SPDR = val;
	return true;
}

/**
 * @brief reads the register <b>reg</b> and writes back to the pointer <val>
 * @param reg the register that should be read
 * @param val the value of the register
 * @returns true on success.
 */
bool spi_read_reg(uint8_t reg, uint8_t * const val){
	cs_gyro2_select();
	SPDR = READ_REG(reg);
	while(!(SPSR & (1<<SPIF)));
	SPDR = SPI_DUMMY;
	//TODO! CHECK IF NEXT LINE REALY WORKS
	while(!(SPSR & (1<<SPIF)));
	cs_gyro2_unselect();
	*val = SPDR;
	return true;
}

