/*
 * ADCops.h
 *
 * Created: 24.04.2021 07:44:21
 *  Author: root
 */ 
#ifndef ADCOPS_H_
#define ADCOPS_H_

#ifndef F_CPU
#define F_CPU 16000000UL // Clock Speed
#endif

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

void ADC_Init(void);
void ADC_Enable(void);
void ADC_Disable(void);
uint16_t ADC_GetEntropy(void);
//uint16_t ADC_ReadVoltage(void);

#endif /* ADCOPS_H_ */