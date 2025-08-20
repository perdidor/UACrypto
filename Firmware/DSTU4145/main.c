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
#include <stdbool.h>

#include "PrivateKey.h"
#include "PubKey.h"
#include "USART.h"
#include "dstu_types.h"
#include "Field.h"
#include "ADCops.h"
#include "PRNG.h"
//#include "ConvertHelper.h"

uint8_t hash_to_sign[32] = { 217, 93, 118, 205, 193, 126, 26, 190, 106, 155, 213, 25, 181, 129, 28, 24, 239, 115, 250, 11, 19, 250, 103, 50, 159, 47, 192, 229, 252, 97, 38, 155 };
	
int main(void)
{
	DDRD |= 0x40;
	PORTD = 0b00000000;

	EXT_UART_Setup();
	char buff[64];
	EXT_UART_Transmit("START PROGRAM\r\n");
	PRNG_Init();
	
	Signer_Setup();

	int cnt = 0;
    /* Replace with your application code */
    while (1) 
    {
		sprintf(buff, "==== PASS #%d ====\r\n", ++cnt);
		EXT_UART_Transmit(buff);
		EXT_UART_Transmit("hash value:\r\n");
		PrintDebugByteArray(hash_to_sign, 32);
		SignHash(hash_to_sign);
		EXT_UART_Transmit("signatureOut value:\r\n");
		PrintDebugByteArray(signatureOut, 64);
		bool sss = VerifySignature(hash_to_sign, signatureOut);
		sprintf(buff, "Signature verify result: %d\r\n", sss);
		EXT_UART_Transmit(buff);
    }
}

