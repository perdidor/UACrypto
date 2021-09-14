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

//#include "PrivateKey.h"
#include "USART.h"
#include "gost89.h"

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

int main(void)
{
	DDRD |= 0x40;
	PORTD = 0b00000000;
	//PORTD ^= 0x40;
	//delay_1ms(1000);
	//PORTD ^= 0x40;
	EXT_UART_Setup();
	//EXT_UART_Transmit("hello!\r\n");
	//uint8_t s[2] = { gost89hash.H[0], gost89hash.H[01] };
	//PrintDebugByteArray(s, 2);
	//EXT_CRLF();
	//uint8_t * sss = DecodeData((uint8_t *)"tect4", 5);
	//uint8_t s[2] = { sss[0], sss[01] };
	//PrintDebugByteArray(sss, 32);
	EXT_UART_Transmit("done!\r\n");
	//bc_init_numbers();
	//dstuprivkey_t * priv = malloc(sizeof(dstuprivkey_t));
	//memcpy(priv, DSTUPrivKeyRawData, 917);
	//uint8_t c = 0;
	//doubletype * param_m_len = malloc(sizeof(doubletype));
	//memcpy(param_m_len->bytes, &DSTUPrivKeyRawData[0], 4);
	//param_m_len->value = 0;
	//
	//priv->param_m.length_W = param_m_len->value;
	//memcpy(priv->param_m.wordv, &DSTUPrivKeyRawData[4], priv->param_m.length_W);
	//EXT_UART_Setup();
	//EXT_UART_Transmit("dfsdfsdfsdfsdf");
	//EXT_UART_Setup();
	int cnt = 0;
    /* Replace with your application code */
    while (1) 
    {
		PORTD ^= 0x40;
		char * buff = malloc(64);
		sprintf(buff, "Hello #%d", cnt++);
		EXT_UART_Transmit(buff);
		EXT_CRLF();
		free(buff);
		//EXT_UART_Transmit("asdasd\r\n");
		delay_1ms(1000);
    }
}

