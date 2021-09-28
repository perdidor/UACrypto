/*
 * DSTU4145.c
 *
 * Created: 31.08.2021 08:48:01
 * Author : root
 */ 

#define F_CPU	16000000UL

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
#include "dstu_types.h"
#include "Field.h"
//#include "notaxTemplate.h"
//#include "ADCops.h"
#include "PRNG.h"
//#include "ConvertHelper.h"

uint8_t ss[54] = { 54, 178, 216, 188, 139, 233, 3, 35, 23, 46, 155, 177, 212, 33, 246, 115, 116, 11, 138, 48, 220, 233, 146, 116, 174, 80, 185, 198, 172, 219, 241, 92, 188, 177, 139, 37, 24, 5, 94, 113, 80, 31, 2, 22, 121, 177, 179, 163, 52, 206, 238, 34, 83, 176 };

uint8_t hashe[32] = { 118, 224, 75, 73, 226, 172, 254, 162, 20, 109, 115, 27, 201, 120, 148, 88, 235, 33, 153, 238, 231, 75, 51, 168, 254, 71, 1, 43, 192, 228, 129, 77 };
	
uint32_t ffb[9] = { 3207353081, 2158610685, 2079352187, 2854537735, 670715314, 2566472407, 2972412005, 3301225405, 0 };

int main(void)
{
	//field_t ff, ff2;
	//FieldFromUint32Buf(ffb, 9, &ff);
	//FieldTruncate(&ff, &ff2);
	DDRD |= 0x40;
	PORTD = 0b00000000;

	EXT_UART_Setup();
	char buff[64];
	EXT_UART_Transmit("START PROGRAM\r\n");
	PRNG_Init();

	SignHash(hashe);

	int cnt = 0;
    /* Replace with your application code */
    while (1) 
    {
		PORTD ^= 0x40;
		
		sprintf(buff, "Hello #%d", cnt++);
		EXT_UART_Transmit(buff);
		EXT_CRLF();
		delay_1ms(1000);
    }
}

