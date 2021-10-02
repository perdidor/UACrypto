/*
 * PRNG.c
 *
 * Created: 12.04.2021 17:27:30
 *  Author: root
 */ 

#define F_CPU	16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include "PRNG.h"
#include "ADCops.h"
#include "USART.h"

uint8_t GetRandomByte() {
	return (uint8_t)(rand() & 0xff);
}

void PRNG_Init() {
	ADC_Init();
	unsigned int rand_seed = 0;
	for (uint8_t i = 0; i < 128; i++)
	{
		rand_seed += ADC_GetEntropy();
	}
	srand(rand_seed);
	//char buff[32];
	//sprintf(buff, "Rand seed %u\r\n", rand_seed);
	//EXT_UART_Transmit(buff);
}