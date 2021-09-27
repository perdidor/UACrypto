/*
 * Test.c
 *
 * Created: 24.09.2021 14:11:23
 * Author : root
 */ 

#define F_CPU	8000000UL

#include <avr/io.h>
#include <util/delay.h>

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
	PORTD ^= 0x40;
    /* Replace with your application code */
    while (1) 
    {
		PORTD ^= 0x40;
		_delay_ms(500);
    }
}

