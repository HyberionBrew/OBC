/**
* @file hal_gyro_2.c
* @author Fabian Kresse, Andreas Sinn
* @date 14 Dezember 2019
* @brief The  Gyro 2 (BMG250) is talked to over SPI
*
* @details like all SPI components it is in the PR VDD
*/

#include "hal_spi.h"
#include <stdbool.h>

/**
 *@brief initializes the gyro 2 (BMG250)
 * @param void
 *@ details First checks device ID, then sets the gyr_config (0x42) register and gyr_range
 *  - GYR_CONF
 *	+ 25 HZ output data rate
 *	+ 10.7 HZ bandwidth
 *  - GYR_RANGE
 *	+ 500 &deg;/s 
 *	+ 15.3 m &deg;/s 
 *	+ LSB
 *
 * 
 *@returns true if init was successful.
*/
bool hal_spi_gyro_2_init(void){
	
	// --- checks if device id register has the default value ---
	uint8_t *val = 0x00;
	spi_read_reg(0x00,val);
	if (*val != 0xD5) //TODO is *?
		{
			//TODO remove next return
			return false;
		}
	return true;
}

/**
 * @brief reads gyroscope data from the gyro 2
 * @returns void
 */
bool hal_gyro2_acc_values(void){
	return true;
}

/**
 * @brief reads temperature data from the gyro 2
 * @returns the temperature of gyro 2 as a uint_8
 */
bool hal_gyro2_temp_value(void){
	return true;
}