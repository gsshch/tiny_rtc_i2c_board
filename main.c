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
#include "adc/adc.h"

/* Dummy debug strings */
static const char Dummy_EEPROM[] = "EEPROM_Dummy_data";
static const char Dummy_RTC_RAM[] = "RTC_RAM_Dummy_data";

/* Timer0 ISR g_tmr0_sec ticker  */
static volatile uint8_t g_tmr0_sec = 0;
/* Timer0 ISR g_tmr0_ticker, overflow ticker */
static volatile uint32_t g_tmr0_ticker = 0;
/* Print flag, g_print, set in INT4 ISR to signal button pressed */
static volatile uint8_t g_print = 0x00;

void led_init(void)
{
        uint8_t PORTB_shadow, DDRB_shadow;

        /* Blinking "keep-alive" LED on Arduino pin 13 (PB7 on Mega) */
        DDRB_shadow = DDRB;
        DDRB = DDRB_shadow | (1 << DDB7);
        PORTB_shadow = PORTB | (1 << PB7);
        PORTB = PORTB_shadow;
}

void led_toggle(void)
{
        /* Blink LED connected to Arduino pin 13 (PB7 on Mega) */
        PINB = PINB | 1<<PINB7;
}

void timer0_init(void)
{
        g_tmr0_sec = 0;
        TCCR0A = 0x00;          /* Normal mode */
        TCCR0B = 0x05;          /* F_CPU/1024 pre-scaling */
        TCNT0 = 0x00;           /* Clear counter 0 */
        TIMSK0 = (1 << TOIE0);  /* Enable overflow IRQ */
}

void button_init(void)
{
        uint8_t DDRE_shadow;

        /* Configure Arduino Mega Pin 2 as IRQ (PE4, INT4) */
        DDRE_shadow = DDRE;
        DDRE = DDRE_shadow & ~(1 << DDE4);      /* PE4 input */
        EICRB |= (1 << ISC41) | (1 << ISC40);   /* IRQ on rising edge */
        EIMSK |= (1 << INT4);                   /* Activate INT4 IRQ */
}

int main(void)
{
        struct rtc_time_var rtc;
        uint8_t timer0_prev_sec;
        char buf[256];

#ifdef APP_ADC_EEPROM
        uint8_t adc_curr = 0;
        uint8_t adc_prev = 0;
        int adc_diff;

        uint16_t eeprom_index = 0;
        uint8_t eeprom_nbr_chars = 0;
#endif
        /* Initialize UART0, serial printing over USB on Arduino Mega */
        uart0_init();

        /* Initialize blinking LED */
        led_init();

        /* Initialize I2C */
        i2c_init();

        /* Initialize Timer0 */
        timer0_init();

        /* Initialize INT4 button */
        button_init();

        /* Initialize ADC */
        adc0_init();

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

        timer0_prev_sec = g_tmr0_sec;
        /* Main loop */
        while (1) {
                if (g_print) {
                        g_print = 0;
#ifdef APP_ADC_EEPROM
                        /* Read out stored EEPROM data upon button-press */
                        eeprom_get_data(0, (uint8_t *)buf, eeprom_index);
                        printf("Stored data[%d]:\n%s\n", eeprom_index, buf);
#else
                        /* Dummy print upon button-press */
                        printf("Button pressed\n");
#endif
                }

                if (timer0_prev_sec == g_tmr0_sec)
                        continue;
                timer0_prev_sec = g_tmr0_sec;
                led_toggle();
                rtc_get_time_var(&rtc);

#ifdef APP_ADC_EEPROM
                /* Poll the current ADC value to see if there is a +/-10%
                 * deviation since the last sample.
                 */
                adc_curr = adc0_get_val_percentage();
                adc_diff = adc_curr - adc_prev;
                adc_prev = adc_curr;
                if (adc_diff > -10 && adc_diff < 10)
                        continue;

                /* Create a char-array containing the RTC-time and the ADC
                 * percentage value and store it in the EEPROM.
                 */
                eeprom_nbr_chars = snprintf(buf, 16, "%d%d:%d%d - %d%%\n",
                                rtc.min_10, rtc.min_1, rtc.sec_10, rtc.sec_1,
                                adc0_get_val_percentage());
                printf("eeprom_index:%d eeprom_nbr_chars:%d %s\n",
                                eeprom_index, eeprom_nbr_chars, buf);
                eeprom_set_data(eeprom_index, (uint8_t *)buf, eeprom_nbr_chars);
                eeprom_index += eeprom_nbr_chars;
#else
                /* Print out the ADC value and the RTC time every second */
                printf("Elapsed RTC time - min:%d%d sec:%d%d\n",
                                rtc.min_10, rtc.min_1, rtc.sec_10, rtc.sec_1);
                printf("Current adc_val:%d %d%%\n\n",
                                adc0_get_val(), adc0_get_val_percentage());
#endif
        }
}

ISR(TIMER0_OVF_vect)
{
        g_tmr0_ticker++;
        if (g_tmr0_ticker % 70 == 0)
                g_tmr0_sec++;
}

ISR(INT4_vect)
{
        static uint32_t tmr0_tck_track = 0;

        /* A simple de-bounce handler where all new IRQs shorter than
         * ~300ms (20/70 * 1 sec) are discarded.
         */
        if (tmr0_tck_track < g_tmr0_ticker)
                g_print = 0x01;
        tmr0_tck_track = g_tmr0_ticker + 20;
}