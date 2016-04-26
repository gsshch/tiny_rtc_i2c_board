/*
 * common.h
 *
 * Created: 2016-04-19 12:43:15
 *  Author: alex.rodzevski
 */ 


#ifndef COMMON_H_
#define COMMON_H_

#ifndef F_CPU
/* 16 MHz clock speed, needs to be defined before including delay.h */
#define F_CPU 16000000UL
#endif
#include <util/delay.h>

#define BAUD    9600

/* TODO: Define RTC 7-bit slave address */
#define DS1307                  (uint8_t)0x68

/* TODO: Define EEPROM 7-bit slave address */
#define AT24C32                 (uint8_t)0x50

/* Un-comment to activate the ADC-EEPROM Reference Application */
//#define APP_ADC_EEPROM

#endif /* COMMON_H_ */