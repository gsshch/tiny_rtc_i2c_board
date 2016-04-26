/*
 * adc.c
 *
 * Created: 2016-04-18 14:08:53
 *  Author: alex.rodzevski
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint16_t adc;

void adc0_init(void)
{	
        adc = 0;	
	DIDR2 = (1 << ADC15D);          /* disable digital input on ADC15 */

	ADMUX	|= (1 << REFS0);        /* set reference voltage (5V) */
        ADMUX	|= (1 << ADLAR);        /* left adjustment of ADC value */
        ADMUX   |= (1 << MUX0);         /* Single Ended Input for ADC15 */
        ADMUX   |= (1 << MUX1);
        ADMUX   |= (1 << MUX2);
        ADMUX   &= ~(1 << MUX3);
        ADMUX   &= ~(1 << MUX4);
        ADCSRB	|= (1 << MUX5);

	ADCSRA |= 7;                    /* prescaler 128 */
	ADCSRA |= (1 << ADATE);         /* enable Auto Trigger */
	ADCSRA |= (1 << ADIE);          /* enable Interrupt */
	ADCSRA |= (1 << ADEN);          /* enable ADC */

	ADCSRA |= (1 << ADSC);          /* start conversion */
}

uint16_t adc0_get_val(void)
{
        return adc;
}

uint16_t adc0_get_val_percentage(void)
{
        uint32_t percentage;
        percentage = ((uint32_t)adc * 100) / 1023;
        /* Error correction, should not return more than 100 */
        if (percentage > 100) {
                percentage = 100;
        }
        return (uint16_t)percentage;
}

/*
 * Interrupt Service Routine for the ADC.
 * The ISR will execute when a A/D conversion is complete.
 */
ISR(ADC_vect)
{
        uint8_t adc_l, adc_h;

        adc_l = 0xB0 & ADCL;
        adc_h = ADCH;
        adc = (uint16_t)(adc_h << 2) | (uint16_t)(adc_l >> 6);
}
