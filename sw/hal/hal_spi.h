/**
* @file hal_spi.c
* @author Andreas Sinn, Fabian Kresse
* @date 17 November 2019
* @brief The SPI HAL interface headerfile
*
*/
#include "config.h"
#include <stdbool.h>

#ifndef SPI_H_2
#define SPI_H_2

//Macros
#define SPI_RX_SIZE 8
#define SPI_READ(reg) (0x80 | (reg))

void hal_spi_init(void);
bool spi_write_reg(uint8_t reg, uint8_t val);
bool spi_read_reg(uint8_t reg, uint8_t* val);

/**
 * @brief struct for a SPI job
 */
typedef struct {
	uint8_t nb; /**< The total number of bytes in transfer (send + receive)*/
	uint8_t cb; /**< Current byte position*/
	
	uint8_t tx[SPI_RX_SIZE+1]; /**< tx*/
	uint8_t rx[SPI_RX_SIZE]; /**< received bytes*/
	uint8_t active; /**< currently active */
	uint8_t done; /**< one if done*/
	} volatile spi_job_t;

/**
 * @brief struct for a gyroscope job
 */
//typedef struct gyro_t{
	//uint8_t x; /**< x- value of gyro*/
	//uint8_t y; /**< y-value of gyro*/
	//uint8_t z; /**< z-value of gyro*/
	//};
	
#endif /* SPI_H_ */