/*
 * main.c
 *
 * Description: This file contains the reference application file Arduino Mega
 * (or similar devices running AVR8 MCUs) targeting the Tiny RTC PCB.
 * This application will use the Arduino twi library to create an i2c-wrapper
 * and implement functionalities which explores the PCBs functions such as
 * real-time clock handling, usage of an internal RAM buffer and storing/reading
 * data from an external EEPROM.
 *
 * Created: 2016-04-05
 * Author : alex.rodzevski@gmail.com
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart/uart.h"

#ifndef F_CPU
/* 16 MHz clock speed, needs to be defined before including delay.h */
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

#define BAUD    9600
#define MYUBRR  (F_CPU/16/BAUD-1)

/* RTC 7-bit slave address */
#define DS1307 0x68

/* EEPROM 7-bit slave address */
#define AT24C32 0x50

/* DS1307 RTC register for start/stop time counter */
#define RTC_REG_START_TIME 0x00

/* DS1307 RTC register address pointing at the internal RAM-buffer */
#define RTC_REG_RAM_BUF_START 0x08

static int uart_putchar(char c, FILE *unused)
{
        if (c == '\n')
                uart_putchar('\r', 0);
        uart0_transmit(c);
        return 0;
}

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
        uint8_t PORTB_shadow, DDRB_shadow;

        /* GPIO toggle-test snippet (Blinking LED on Arduino Mega) */
        DDRB_shadow = DDRB;
        DDRB = DDRB_shadow | (1 << DDB7);
        PORTB_shadow = PORTB | (1 << PB7);
        PORTB = PORTB_shadow;

        /* Re-rout stdout (printf) to use internal uart_putchar */
        stdout = &mystdout;
        /* Initialize UART0, serial printing over USB on Arduino Mega */
        uart0_init(MYUBRR);
        _delay_ms(1000);

        printf("The Tiny RTC firmware has started\n");

        /* Main loop */
        while (1) {
                _delay_ms(1000);
                /* Blink Arduino LED connected to pin 13 (PB7 on Mega) */
                PINB = PINB | 1<<PINB7;
                printf("UART and LED blink is working!\n");
        }
}

