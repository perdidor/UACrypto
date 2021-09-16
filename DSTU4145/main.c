/*
 * DSTU4145.c
 *
 * Created: 31.08.2021 08:48:01
 * Author : root
 */ 

#define F_CPU	8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>

#include "PrivateKey.h"
#include "USART.h"
#include "gost89.h"
#include "notaxTemplate.h"
#include "ADCops.h"
#include "PRNG.h"
#include "ConvertHelper.h"

void delay_1ms(uint16_t ms) {
	volatile uint16_t i,foo = 0;
	for(i=0;i<ms;i++)
	{
		_delay_ms(1);
		//make some "work" to avoid optimization
		foo++;
	}
	foo = 0;
}

uint8_t ss[54] = { 54, 178, 216, 188, 139, 233, 3, 35, 23, 46, 155, 177, 212, 33, 246, 115, 116, 11, 138, 48, 220, 233, 146, 116, 174, 80, 185, 198, 172, 219, 241, 92, 188, 177, 139, 37, 24, 5, 94, 113, 80, 31, 2, 22, 121, 177, 179, 163, 52, 206, 238, 34, 83, 176 };

int main(void)
{
	DDRD |= 0x40;
	PORTD = 0b00000000;
	//PORTD ^= 0x40;
	//delay_1ms(1000);
	//PORTD ^= 0x40;
	EXT_UART_Setup();
	//ADC_Init();
	//PRNG_Init();
	uint8_t * as = InvertByteArray(ss, 54);
	PrintDebugByteArray(as, 55);
	//ADC_Enable();
	//uint16_t qwe = ADC_GetEntropy();
	//char * buff2 = malloc(64);
	//sprintf(buff2, "ADC %d", qwe);
	//EXT_UART_Transmit(buff2);
	//EXT_CRLF();
	//free(buff2);
	int cnt = 0;
    /* Replace with your application code */
    while (1) 
    {
		PORTD ^= 0x40;
		//char * buff = malloc(64);
		//sprintf(buff, "Hello #%d", cnt++);
		//EXT_UART_Transmit(buff);
		//EXT_CRLF();
		//free(buff);
		delay_1ms(1000);
    }
}

