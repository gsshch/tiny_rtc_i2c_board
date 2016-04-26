/*
 * adc.h
 *
 * Created: 2016-04-18 14:09:09
 *  Author: alex.rodzevski
 */ 


#ifndef ADC_H_
#define ADC_H_

void adc0_init(void);
uint16_t adc0_get_val(void);
uint16_t adc0_get_val_percentage(void);


#endif /* ADC_H_ */