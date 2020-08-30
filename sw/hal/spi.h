/*
 * spi.h
 *
 * Created: 02.06.2017 
 *  Author: asinn
 */ 

#include "config.h"

#ifndef SPI_H_
#define SPI_H_


void spi_init(void);
void spi_process(uint8_t new_routine);
void gyro1_init(void);
void gyro2_init(void);
void mag2_init(void);

void gyro2_read_temperature(void);
void gyro1_read_temperature(void);
void gyro1_read_values(void);
void gyro2_read_values(void);

#endif /* SPI_H_ */