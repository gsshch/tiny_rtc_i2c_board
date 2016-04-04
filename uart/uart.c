/*
 * File name: uart.c
 * 
 * Description: A rudimentary UART driver for the ATMega 2560 chip
 *
 * Created: 2016-04-05
 * Author: alex.rodzevski@gmail.com
 */ 

#include <avr/io.h>

void uart0_init(unsigned int ubrr)
{
        /* Set baud rate */
        UBRR0H = (unsigned char)(ubrr>>8);
        UBRR0L = (unsigned char)ubrr;
        /* Enable receiver and transmitter */
        UCSR0B = (1<<RXEN0)|(1<<TXEN0);
        /* Set frame format: Async, No parity, 1 stop bit, 8 data */
        UCSR0C = (3<<UCSZ00);
}


void uart0_transmit(unsigned char data)
{
        /* Wait for empty transmit buffer */
        while (!( UCSR0A & (1<<UDRE0)));
        /* Start transmission by loading data into the buffer */
        UDR0 = data;
}