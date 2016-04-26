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
#include "common.h"
#include "uart/uart.h"
#include "i2c/i2c.h"
#include "rtc/rtc.h"
#include "eeprom/eeprom.h"

/* Dummy debug strings */
static const char Dummy_EEPROM[] = "EEPROM_Dummy_data";
static const char Dummy_RTC_RAM[] = "RTC_RAM_Dummy_data";



int main(void)
{
        uint8_t PORTB_shadow, DDRB_shadow;
        struct rtc_time_var rtc;
        char buf[256];

        /* GPIO toggle-test snippet (Blinking LED on Arduino Mega) */
        DDRB_shadow = DDRB;
        DDRB = DDRB_shadow | (1 << DDB7);
        PORTB_shadow = PORTB | (1 << PB7);
        PORTB = PORTB_shadow;

        /* Initialize UART0, serial printing over USB on Arduino Mega */
        uart0_init();

        /* Initialize I2C */
        i2c_init();

        /* Enable global interrupts (used in I2C) */
        sei();

        /* Added startup delay after IRQ-enable and followed by a boot print */
        _delay_ms(1000);
        printf("\n\nTiny RTC firmware successfully started!\n\n");
        printf("RTC DS1307 I2C-addr:0x%x\n", DS1307);
        printf("EEPROM AT24C32 I2C-addr:0x%x\n\n", AT24C32);

        rtc_init();

        /* Write dummy data to the RTC RAM */
        rtc_set_ram_buf((uint8_t *)Dummy_RTC_RAM, strlen(Dummy_RTC_RAM));

        /* Write dummy data to the EEPROM */
        eeprom_set_data(0, (uint8_t *)Dummy_EEPROM, strlen(Dummy_EEPROM));

        _delay_ms(1000);

        /* Read and print dummy data from RTC RAM  */
        memset(buf, 0, sizeof(buf));
        rtc_get_ram_buf((uint8_t *)buf, strlen(Dummy_RTC_RAM));
        printf("RTC RAM read result:%s\n", buf);

        /* Read and print dummy data from EEPROM  */
        memset(buf, 0, sizeof(buf));
        eeprom_get_data(0, (uint8_t *)buf, strlen(Dummy_EEPROM));
        printf("EEPROM read result:%s\n\n", buf);

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