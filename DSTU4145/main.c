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
#include "notaxTemplate.h"
#include "gost89.h"
//#include "ADCops.h"
#include "PRNG.h"
//#include "ConvertHelper.h"

uint8_t ss[54] = { 54, 178, 216, 188, 139, 233, 3, 35, 23, 46, 155, 177, 212, 33, 246, 115, 116, 11, 138, 48, 220, 233, 146, 116, 174, 80, 185, 198, 172, 219, 241, 92, 188, 177, 139, 37, 24, 5, 94, 113, 80, 31, 2, 22, 121, 177, 179, 163, 52, 206, 238, 34, 83, 176 };

uint8_t dsh[32] = { 98, 233, 213, 186, 106, 214, 97, 109, 176, 103, 200, 209, 166, 209, 121, 106, 49, 227, 3, 22, 83, 154, 187, 45, 94, 42, 58, 132, 180, 53, 85, 46 };

uint8_t msgdigest[32] = { 115, 46, 87, 204, 244, 171, 26, 186, 51, 54, 101, 13, 176, 239, 71, 43, 163, 100, 220, 143, 59, 25, 163, 249, 248, 243, 67, 14, 90, 181, 1, 29 };

uint8_t hashe[32] = { 217, 93, 118, 205, 193, 126, 26, 190, 106, 155, 213, 25, 181, 129, 28, 24, 239, 115, 250, 11, 19, 250, 103, 50, 159, 47, 192, 229, 252, 97, 38, 155 };
	
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
	NoTaxPrepareDataToSign(msgdigest);
	Gost89HashCompute(DataToSign, DataToSignLength);
	PrintDebugByteArray(gost89Hash_Value, 32);
	//uint8_t pizda[32];
	memcpy(hashe, gost89Hash_Value, 32);
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

