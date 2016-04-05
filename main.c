/*
 * main.c
 *
 * Description: This file contains the reference application file Arduino Mega
 * (or similar devices running AVR8 MCUs) targeting the Tiny RTC PCB.
 * This application will use the Arduino twi-library to create an i2c-wrapper
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
#include "i2c/twi.h"
#include "uart/uart.h"

#ifndef F_CPU
/* 16 MHz clock speed, needs to be defined before including delay.h */
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

#define BAUD    9600
#define MYUBRR  (F_CPU/16/BAUD-1)

/* RTC 7-bit slave address */
#define DS1307                  (uint8_t)0x68
/* EEPROM 7-bit slave address */
#define AT24C32                 (uint8_t)0x50
/* DS1307 RTC register for start/stop time counter */
#define RTC_REG_START_TIME      (uint8_t)0x00
/* DS1307 RTC register address pointing at the internal RAM-buffer */
#define RTC_REG_RAM_BUF_START   (uint8_t)0x08
/* I2C driver flag bits */
#define RTC_RAM_SIZE            (uint8_t)56     /* Bytes */
#define EEPROM_SIZE             (uint16_t)4096   /* Bytes */
#define I2C_WAIT                (uint8_t)1
#define I2C_STOP_BIT            (uint8_t)1

/* RTC time variable struct */
struct rtc_time_var {
        uint8_t sec_10;
        uint8_t sec_1;
        uint8_t min_10;
        uint8_t min_1;
};

/* Dummy debug strings */
static const char Dummy_EEPROM[] = "EEPROM_Dummy_data";
static const char Dummy_RTC_RAM[] = "RTC_RAM_Dummy_data";


static int uart_putchar(char c, FILE *unused)
{
        if (c == '\n')
                uart_putchar('\r', 0);
        uart0_transmit(c);
        return 0;
}

FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

static void rtc_init(void)
{
        uint8_t rtc_reg[RTC_RAM_SIZE+1];

        /* Clear the RTC RAM buffer */
        rtc_reg[0] = RTC_REG_RAM_BUF_START;
        memset(&rtc_reg[1], 0, RTC_RAM_SIZE);
        twi_writeTo(DS1307, rtc_reg, (RTC_RAM_SIZE+1), I2C_WAIT, I2C_STOP_BIT);

        /* Start the RTC clock counter */
        rtc_reg[0] = RTC_REG_START_TIME;
        rtc_reg[1] = 0;
        twi_writeTo(DS1307, rtc_reg, 2, I2C_WAIT, I2C_STOP_BIT);
}

static void rtc_get_time_var(struct rtc_time_var *var)
{
	uint8_t sec, min, rtc_dat[2];

	rtc_dat[0] = RTC_REG_START_TIME;
	twi_writeTo(DS1307, rtc_dat, 1, I2C_WAIT, I2C_STOP_BIT);
	twi_readFrom(DS1307, rtc_dat, 2, I2C_STOP_BIT);
	sec = rtc_dat[0];
	min = rtc_dat[1];

	var->sec_1 = (sec & 0x0F);
	var->sec_10 = ((sec & 0x70) >> 4);
	var->min_1 = (min & 0x0F);
	var->min_10 = ((min & 0x70) >> 4);
}

static uint8_t rtc_get_ram_buf(uint8_t *buf, uint8_t len)
{
        uint8_t ram_addr;

        if (len >= RTC_RAM_SIZE)
                return 0;

        ram_addr = RTC_REG_RAM_BUF_START;
        twi_writeTo(DS1307, &ram_addr, 1, I2C_WAIT, I2C_STOP_BIT);
        twi_readFrom(DS1307, buf, len, I2C_STOP_BIT);
        return len;
}

static uint8_t rtc_set_ram_buf(uint8_t *buf, uint8_t len)
{
        uint8_t ram_buf[RTC_RAM_SIZE + 1];

        if (len >= RTC_RAM_SIZE)
                return 0;

        ram_buf[0] = RTC_REG_RAM_BUF_START;
        strncpy((char*)&ram_buf[1], (const char *)buf, len);
        twi_writeTo(DS1307, ram_buf, (len + 1), I2C_WAIT, I2C_STOP_BIT);
        return len;
}

static uint8_t eeprom_get_data(uint16_t reg_idx, uint8_t *buf, uint16_t len)
{
        if (len >= EEPROM_SIZE)
                return 0;

        /* TODO: add page handling */
        twi_writeTo(AT24C32, (uint8_t *)&reg_idx, 2, I2C_WAIT, I2C_STOP_BIT);
        twi_readFrom(AT24C32, buf, len, I2C_STOP_BIT);
        return len;
}

static uint8_t eeprom_set_data(uint16_t reg_idx, uint8_t *buf, uint8_t len)
{
        if (len >= EEPROM_SIZE)
        return 0;

        /* TODO: add page handling */
        twi_writeTo(AT24C32, (uint8_t *)&reg_idx, 2, I2C_WAIT, I2C_STOP_BIT);
        twi_readFrom(AT24C32, buf, len, I2C_STOP_BIT);
        return len;
}

int main(void)
{
        uint8_t PORTB_shadow, DDRB_shadow;
        struct rtc_time_var rtc;
        uint8_t i2c_buf[32];

        /* GPIO toggle-test snippet (Blinking LED on Arduino Mega) */
        DDRB_shadow = DDRB;
        DDRB = DDRB_shadow | (1 << DDB7);
        PORTB_shadow = PORTB | (1 << PB7);
        PORTB = PORTB_shadow;

        /* Re-rout stdout (printf) to use internal uart_putchar */
        stdout = &mystdout;
        /* Initialize UART0, serial printing over USB on Arduino Mega */
        uart0_init(MYUBRR);

        /* Initialize TWI */
        twi_init(F_CPU);

        /* Enable global interrupts (used in TWI) */
        sei();
        _delay_ms(1000);

        rtc_init();
        /* Write some dummy data to the RTC RAM & the EEPROM */
        rtc_set_ram_buf((uint8_t *)Dummy_RTC_RAM, strlen(Dummy_RTC_RAM));
        eeprom_set_data(0x0000, (uint8_t *)Dummy_EEPROM, strlen(Dummy_EEPROM));

        /* Print the 7-bit I2C-client addresses */
        printf("RTC DS1307 I2C-addr:0x%x\n", DS1307);
        printf("EEPROM AT24C32 I2C-addr:0x%x\n\n", AT24C32);
        _delay_ms(1000);

        /* Read the Dummy data from RTC RAM & the EEPROM and print it */
        memset(i2c_buf, 0, sizeof(i2c_buf));
        rtc_get_ram_buf(i2c_buf, strlen(Dummy_RTC_RAM));
        printf("RTC RAM read result:%s\n", i2c_buf);
        memset(i2c_buf, 0, sizeof(i2c_buf));
        eeprom_get_data(0x0000, i2c_buf, strlen(Dummy_EEPROM));
        printf("EEPROM read result:%s\n\n", i2c_buf);

        /* Main loop */
        while (1) {
                _delay_ms(1000);
                /* Blink LED connected to Arduino pin 13 (PB7 on Mega) */
                PINB = PINB | 1<<PINB7;
		rtc_get_time_var(&rtc);
		printf("Elapsed RTC time - min:%d%d sec:%d%d\n",
			rtc.min_10, rtc.min_1, rtc.sec_10, rtc.sec_1);
        }
}
