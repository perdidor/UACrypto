/*
 * USART.c
 *
 * Created: 31.08.2021 10:06:03
 *  Author: root
 */ 
#ifndef F_CPU
#define F_CPU       8000000UL
#endif
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "USART.h"

void EXT_UART_Setup()
{
	UBRR0H = (MYUBRR) >> 8;
	UBRR0L = MYUBRR;
	UCSR0C |= (1 << UCSZ00)|(1 << UCSZ01);
	UCSR0B = (1 << TXEN0)|(1 << RXEN0);
	//sei();
}

void EXT_UART_Transmit(char * data)
{
	for (int i = 0; i < strlen(data); i++){
		while (( UCSR0A & (1<<UDRE0))  == 0){};
		if ((data[i] >= 30 && data[i] != 127) || (data[i] == 0x0d || data[i] == 0x0a)) UDR0 = data[i];
	}
}

void EXT_CRLF()
{
	EXT_UART_Transmit("\r\n");
}

void PrintDebugByteArray(uint8_t * outarr, size_t arrlen)
{
	char buff[16];
	sprintf(buff, "Bytes: %d\r\nHEX:", arrlen);
	EXT_UART_Transmit(buff);
	for (int a = 0; a < arrlen - 1; a++){
		sprintf(buff, " %02x", outarr[a]);
		EXT_UART_Transmit(buff);
	}
	sprintf(buff, " %02x", outarr[arrlen - 1]);
	EXT_UART_Transmit(buff);
	EXT_CRLF();
}

void EXT_UART_TransmitRawData(uint8_t * outarr, size_t arrlen) {
	for (int i = 0; i < arrlen; i++){
		while (( UCSR0A & (1<<UDRE0))  == 0){};
		UDR0 = outarr[i];
	}
}

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

void PrintDebugUInt32Array(uint32_t * arr, uint32_t arrlen, int passnum)
{
	char buff[32];
	if (passnum >= 0)
	{
	
		sprintf(buff, "MultPos rounds left: %d\r\n", passnum);
		EXT_UART_Transmit(buff);
	}
	for (int a = 0; a < arrlen; a++){
		uint32_t asdasd = 0;
		memcpy(&asdasd, &arr[a], sizeof(uint32_t));
		sprintf(buff, " %lu,\r\n", asdasd);
		//EXT_CRLF();
		EXT_UART_Transmit(buff);
	}
	//sprintf(buff, " %d }", arr[arrlen - 1]);
	//EXT_UART_Transmit(buff);
	//EXT_CRLF();
}