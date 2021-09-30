/*
 * ADCops.c
 *
 * Created: 24.04.2021 07:44:38
 *  Author: root
 */ 
#include "ADCops.h"
#include <avr/sfr_defs.h>

 void ADC_Init()
{
	ADCSRA = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2); /* Enable ADC , sampling freq=osc_freq/128 */
}

void ADC_Enable()
{
	ADCSRA |= (1<<ADEN); // Enable ADC
}

void ADC_Disable()
{
	ADCSRA |= (0<<ADEN); // Disable ADC
}

uint16_t ADC_GetEntropy()
{
	ADMUX |= (0<<REFS0) | (0<<REFS0) | (0<<MUX1) | (0<<MUX2); // select AVCC as reference, use ADC channel 0 for entropy collection.
	_delay_ms(1);
	ADCSRA |= (1 << ADSC);
	loop_until_bit_is_clear(ADCSRA, ADSC);
	return(ADC);
}