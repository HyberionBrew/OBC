/*
 * hw_defs.h
 *
 * Created: 22.11.2016 15:09:45
 *  Author: asinn
 */ 


#ifndef HW_DEFS_H_
#define HW_DEFS_H_

// --- Hardware definitions ---
#ifdef HW_VER_EM2_0

/* --- SPI - Chip select lines --- */
#define CS_GYRO2_PIN		6
#define CS_GYRO2_PORT		B
#define CS_MAG2_PIN			6
#define CS_MAG2_PORT		L
#define CS_FLASH_PIN		0
#define CS_FLASH_PORT		B

#define SPI_SCK_PIN			1
#define SPI_MISO_PIN		3
#define SPI_MOSI_PIN		2
#define SPI_PORT			B

/* --- RADIO UART --- */
#define RADIO1_RX_PIN			2
#define RADIO1_RX_PORT			D	// RXD1
#define RADIO1_TX_PIN			3
#define RADIO1_TX_PORT			D	// TXD1


#define RADIO2_RX_PIN			0
#define RADIO2_RX_PORT			H	// RXD2
#define RADIO2_TX_PIN			1
#define RADIO2_TX_PORT			G	// TXD2

#define RADIO2_IO_PIN 4
#define RADIO2_IO_PORT B

#define BL_SEL1_PIN	3	// Input
#define BL_SEL1_PORT E

#define VCC_MCU_DISABLE_PIN	5
#define VCC_MCU_DISABLE_PORT E

#define TOGGLE_BOOTLOADER_PIN	6
#define TOGGLE_BOOTLOADER_PORT	E

#define CLKO_PIN	7
#define CLKO_PORT E

#define SCL_PIN 0
#define SCL_PORT D
#define SDA_PIN	1
#define SDA_PORT D

#define BAT_OK_PIN	7
#define BAT_OK_PORT	K

#define POWERCYCLE_ENABLE_PIN	1
#define POWERCYCLE_ENABLE_PORT	K
#define WATCHDOG_ENABLE_PIN		0
#define WATCHDOG_ENABLE_PORT	K

#define FRAM_WRITE_ENABLE_PIN	5
#define FRAM_WRITE_ENABLE_PORT	J
#define RESET_BY_EXT_WDT_PIN	7
#define RESET_BY_EXT_WDT_PORT	J

#define MAG2_RESET_PIN	0
#define MAG2_RESET_PORT	L

#define DEBUG_SEL1_PIN	1
#define DEBUG_SEL1_PORT	L
#define DEBUG_SEL2_PIN	2
#define DEBUG_SEL2_PORT	L

#define POWERCYCLE_12V_PIN	4
#define POWERCYCLE_12V_PORT	L
#define POWERCYCLE_3V3_PIN	6
#define POWERCYCLE_3V3_PORT	K

#define RADIO1_IO_PIN		5
#define RADIO1_IO_PORT		L

/* --- VDD status and control --- */
#define VDD_DISABLE_PIN			5
#define VDD_DISABLE_PORT		B
#define VDD_FAULT_PIN			7
#define VDD_FAULT_PORT			B

#define VEE_DISABLE_PIN			5
#define VEE_DISABLE_PORT		D
#define VEE_FAULT_PIN			3
#define VEE_FAULT_PORT			L

#define WATCHDOG_FEED_PIN		4
#define WATCHDOG_FEED_PORT		E

#define RBF_PIN					7
#define RBF_PORT				L

/* --- External memory --- */
#define A16_PIN					7
#define A16_PORT				D
#define A17_PIN					4
#define A17_PORT				D
				
/* --- Analog input pins --- */
#define SUPPLY_CURRENT_PIN		0
#define SUPPLY_CURRENT_PORT		F
#define SUPPLY_CURRENT_ADC_CH	0	

#define VCC_MEASURE_PIN			1
#define VCC_MEASURE_PORT		F
#define VCC_MEASURE_ADC_CH		1

#define TEMPERATURE_PIN			2
#define TEMPERATURE_PORT		F
#define TEMPERATURE_ADC_CH		2

/* --- External software SPI --- */
#define EXT_SCK_PIN				2
#define EXT_SCK_PORT			J	
#define EXT_NCS_PIN				4
#define EXT_NCS_PORT			J	
#define EXT_MOSI_PIN			1	
#define EXT_MOSI_PORT			J	
#define EXT_MISO_PIN			0	
#define EXT_MISO_PORT			J	

/* --- EPS - ADC --- */
#define ADC_EOC_PIN				6
#define ADC_EOC_PORT			J



/* --- Magnetic torquer --- */
#define MT_ENABLE_PIN			6
#define MT_ENABLE_PORT			D

#define MT_PWM_R1_PIN			2
#define MT_PWM_R1_PORT			E			
#define MT_PWM_F1_PIN			3
#define MT_PWM_F1_PORT			E	
#define MT_PWM_R2_PIN			4
#define MT_PWM_R2_PORT			E	
#define MT_PWM_F2_PIN			5
#define MT_PWM_F2_PORT			E	
#define MT_PWM_R3_PIN			6
#define MT_PWM_R3_PORT			E	
#define MT_PWM_F3_PIN			7	
#define MT_PWM_F3_PORT			E	

/* --- RS485 --- */
#define RS485_TX_PIN			1
#define RS485_TX_PORT			E	// USART 0
#define RS485_RX_PIN			0
#define RS485_RX_PORT			E	// USART 0

#define RS485_TX_NRX_PIN		2
#define RS485_TX_NRX_PORT		F	// TX enable, not RX enable

#elif defined(HW_VER_EM1_1)

#else
#error "Hardware version does not exist!"
#endif




#endif /* HW_DEFS_H_ */