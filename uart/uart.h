/*
 * File name: uart.h
 *
 * Description: Header file for the driver implemented in uart.c
 *
 * Created: 2016-04-05
 * Author: alex.rodzevski@gmail.com
 */ 

#ifndef UART_H_
#define UART_H_

void uart0_init(unsigned int ubrr);
void uart0_transmit(unsigned char data);

#endif /* UART_H_ */