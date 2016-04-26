/*
 * File name: uart.c
 * 
 * Description: A rudimentary UART driver for the ATMega 2560 chip
 *
 * Created: 2016-04-05
 * Author: alex.rodzevski@gmail.com
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "../common.h"
#include "uart.h"

#define MYUBRR  (unsigned int)(F_CPU/16/BAUD-1)

static int uart_putchar(char c, FILE *unused)
{
        if (c == '\n')
        uart_putchar('\r', 0);
        uart0_transmit(c);
        return 0;
}

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void uart0_init(void)
{
        /* Set baud rate */
        UBRR0H = (unsigned char)(MYUBRR >> 8);
        UBRR0L = (unsigned char)MYUBRR;
        /* Enable receiver and transmitter */
        UCSR0B = (1<<RXEN0)|(1<<TXEN0);
        /* Set frame format: Async, No parity, 1 stop bit, 8 data */
        UCSR0C = (3<<UCSZ00);

        /* Re-rout stdout (printf) to use internal uart_putchar */
        stdout = &mystdout;
}


void uart0_transmit(unsigned char data)
{
        /* Wait for empty transmit buffer */
        while (!( UCSR0A & (1<<UDRE0)));
        /* Start transmission by loading data into the buffer */
        UDR0 = data;
}