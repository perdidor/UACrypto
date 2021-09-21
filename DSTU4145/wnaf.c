/*
 * wnaf.c
 *
 * Created: 08.09.2021 13:46:18
 *  Author: root
 */ 
#include "gf2m.h"
#include "wnaf.h"
#include "dstu_types.h"
#include "Curve.h"
#include <math.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

uint32_t DEFAULT_CUTOFFS[] = { 13, 41, 121, 337, 897, 2305 };

uint32_t windowNaf(uint32_t width, field_t * field, int32_t * resarray) {
	field_t f;
	FieldClone(field, &f);
	uint32_t f_len = FieldBitLength(&f);
	uint32_t ret_len = floor(f_len / width + 1);
	//resarray = malloc(ret_len * sizeof(int32_t));
	uint32_t pow2 = 1 << width;
	uint32_t masbigint = pow2 - 1;
	uint32_t sign = pow2 >> 1;

	bool carry = false;
	int32_t length = 0, pos = 0;
	int32_t digit = 0, zeroes = 0;

	while (pos <= f_len && length < ret_len)
	{
		if (FieldTestBit(pos, &f) == carry)
		{
			++pos;
			continue;
		}

		FieldShiftRightM(&f, pos);

		digit = (f.bytes[0] & masbigint);

		if (carry == 1)
		{
			++digit;
		}

		carry = (digit & sign) != 0;
		if (carry)
		{
			digit -= pow2;
		}

		zeroes = ((length > 0) ? (pos - 1) : pos);
		resarray[length] = (int32_t)((int32_t)digit << 16) | (int32_t)zeroes;
		length++;
		pos = width;
	}

	//if (ret_len > length) {
		//return length;
	//}

	return length;
}

uint32_t getWindowSize(uint32_t bits) {
	uint8_t idx = 0;
	for ( ;idx < 6; idx++)
	{
		if (bits < DEFAULT_CUTOFFS[idx]) {
			break;
		}
	}

	return idx + 2;
}