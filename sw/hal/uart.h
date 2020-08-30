/*
 * uart.h
 *
 * Created: 31.05.2017 17:20:57
 *  Author: asinn
 */ 


#ifndef UART_H_
#define UART_H_

#include "config.h"

#define RADIO_UBRR_VAL ((F_CPU / 16) / RADIO_BAUD_RATE - 1)
#define RADIO_BAUD_RATE_REAL (F_CPU / (16 * ((RADIO_UBRR_VAL) + 1)))
#define RADIO_BAUD_RATE_ERROR (100* RADIO_BAUD_RATE_REAL / RADIO_BAUD_RATE - 100)

#if (RADIO_BAUD_RATE_ERROR > 5) || (RADIO_BAUD_RATE_ERROR < -5)
#error "Radio baud rate error!"
#endif


/* USART Buffer Defines */
#define USART_RX_BUFFER_SIZE 32     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_TX_BUFFER_SIZE 32     /* 2,4,8,16,32,64,128 or 256 bytes */
#define USART_RX_BUFFER_MASK (USART_RX_BUFFER_SIZE - 1)
#define USART_TX_BUFFER_MASK (USART_TX_BUFFER_SIZE - 1)

#if (USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK)
#error RX buffer size is not a power of 2
#endif
#if (USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK)
#error TX buffer size is not a power of 2
#endif

void uart_radio_init(void);
RetVal radio_transmit (char * s, uint8_t len);
RetVal radio_printf(const char * format, ...);
uint8_t radio_receive(char *rx, uint8_t maxlen);

#endif /* UART_H_ */