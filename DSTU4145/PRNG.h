/*
 * PRNG.h
 *
 * Created: 12.04.2021 17:27:15
 *  Author: root
 */ 
#ifndef PRNG_H_
#define PRNG_H_

#ifndef F_CPU
#define F_CPU 16000000UL // Clock Speed
#endif

#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>

void PRNG_Init(void);
uint8_t GetRandomByte(void);

#endif /* PRNG_H_ */