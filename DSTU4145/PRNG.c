/*
 * PRNG.c
 *
 * Created: 12.04.2021 17:27:30
 *  Author: root
 */ 
#include "PRNG.h"
#include "ADCops.h"

uint8_t GetRandomByte() {
	return (uint8_t)(rand() & 0xff);
}

void PRNG_Init() {
	uint16_t rand_seed = 0;
	for (uint8_t i = 0; i < 65; i++)
	{
		rand_seed += ADC_GetEntropy();
	}
	srand(rand_seed);
}