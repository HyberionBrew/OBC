/*
* uart.c
*
* Created: 31.05.2017 17:17:22
*  Author: asinn
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#include "uart.h"


/* Static Variables */
static unsigned char USART_RxBuf[USART_RX_BUFFER_SIZE];
static volatile unsigned char USART_RxHead;
static volatile unsigned char USART_RxTail;
static unsigned char USART_TxBuf[USART_TX_BUFFER_SIZE];
static volatile unsigned char USART_TxHead;
static volatile unsigned char USART_TxTail;

static RetVal uart_radio_transmit_byte(unsigned char data);
static unsigned char uart_radio_receive_byte(void);
static inline bool uart_radio_check_available(void);

/* Initialize USART */
void uart_radio_init(void)
{
	unsigned char x;

	/* Set the baud rate */
	UBRR1H = (unsigned char) (RADIO_UBRR_VAL>>8);
	UBRR1L = (unsigned char) RADIO_UBRR_VAL;
	
	/* Enable USART receiver and transmitter */
	UCSR1B = ((1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1)); 
	
	/* Set frame format: 8 data 1stop */
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	
	/* Flush receive buffer */
	x = 0;

	USART_RxTail = x;
	USART_RxHead = x;
	USART_TxTail = x;
	USART_TxHead = x;
}

ISR(USART1_RX_vect)
{
	unsigned char data;
	unsigned char tmphead;

	/* Read the received data */
	data = UDR1;
	/* Calculate buffer index */
	tmphead = (USART_RxHead + 1) & USART_RX_BUFFER_MASK;
	/* Store new index */
	USART_RxHead = tmphead;

	if (tmphead == USART_RxTail) {
		/* ERROR! Receive buffer overflow */
	}
	/* Store received data in buffer */
	USART_RxBuf[tmphead] = data;
}

ISR(USART1_UDRE_vect)
{
	unsigned char tmptail;

	/* Check if all data is transmitted */
	if (USART_TxHead != USART_TxTail)
	{
		/* Calculate buffer index */
		tmptail = (USART_TxTail + 1) & USART_TX_BUFFER_MASK;
		/* Store new index */
		USART_TxTail = tmptail;
		/* Start transmission */
		UDR1 = USART_TxBuf[tmptail];
	}
	else
	{
		/* Disable UDRE interrupt */
		UCSR1B &= ~(1<<UDRIE1);
	}
}

static unsigned char uart_radio_receive_byte(void)
{
	unsigned char tmptail;
	
	/* Wait for incoming data */
	while (USART_RxHead == USART_RxTail);
	/* Calculate buffer index */
	tmptail = (USART_RxTail + 1) & USART_RX_BUFFER_MASK;
	/* Store new index */
	USART_RxTail = tmptail;
	/* Return data */
	return USART_RxBuf[tmptail];
}

static inline bool uart_radio_check_available(void)
{
	if(USART_RxHead != USART_RxTail)
	{
		// Data available
		return 1;
	}
	
	return 0;
}

uint8_t radio_receive(char *rx, uint8_t maxlen)
{
	uint8_t len = 0;
	
	while(uart_radio_check_available() == 1)
	{
		rx[len++] = uart_radio_receive_byte();
		if(len == maxlen)
		{
			break;
		}
	}
	
	return len;	
}


static RetVal uart_radio_transmit_byte(unsigned char data)
{
	unsigned char tmphead;
	uint8_t counter;
		
	/* Calculate buffer index */
	tmphead = (USART_TxHead + 1) & USART_TX_BUFFER_MASK;
	/* Wait for free space in buffer */
	counter = 255;
	while (tmphead == USART_TxTail)
	{
		if(counter == 0)
		{
			return FAILED;
		}
		
		counter--;
	}
	/* Store data in buffer */
	USART_TxBuf[tmphead] = data;
	/* Store new index */
	USART_TxHead = tmphead;
	/* Enable UDRE interrupt */
	UCSR1B |= (1<<UDRIE1);
	
	return DONE;
}

RetVal radio_transmit (char * s, uint8_t len)
{
	if(len == 0)
	{
		len = strlen(s);
	}
	
	while (len != 0)
	{
		if(uart_radio_transmit_byte (*s++) == FAILED)
		{
			return FAILED;
		}
		len--;
	}
	
	return DONE;
}

RetVal radio_printf(const char * format, ...)
{
	int len;
	va_list args;
	va_start(args, format);

	// determine how long the result will be
	len = vsnprintf(NULL, 0, format, args);

	// reserve the string and generate it.
	char tx_buf[len + 1];
	vsprintf(tx_buf, format, args);
	
	va_end(args);
	return radio_transmit(tx_buf, 0);
}
