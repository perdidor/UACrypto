/*
 * Field.c
 *
 * Created: 04.09.2021 17:56:36
 *  Author: root
 */ 
#include "Curve.h"
#include "Point.h"
#include "gf2m.h"
#include "dstu_types.h"
#include "Field.h"
#include "ConvertHelper.h"
#include "PRNG.h"
#include "PrivateKey.h"
#include "USART.h"
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

void FieldFromCurve(field_t * newfield) {
	PORTD ^= 0x40;
	memset(newfield->bytes, 0x00, 22 * sizeof(uint32_t));
	newfield->length = Priv_mod_words;
	newfield->_is_field = true;
}

void FieldFromHexStr(char * in_value, field_t * newfield) {
	newfield->length = ConvertHex2Bin(in_value, newfield->bytes);
	newfield->_is_field = true;
}

void FieldFromByteArray(uint8_t * in_value, size_t len, size_t max_size, field_t * res) {
	PORTD ^= 0x40;
	size_t tmpsize = ceil(len / 4);
	tmpsize = (tmpsize > max_size ? tmpsize : max_size);
	memset(res->bytes, 0x00, 32 * sizeof(uint32_t));
	uint32_t code = 0;
	uint32_t bpos = 0;
	uint32_t vidx = 0;
	uint32_t idx = len - 1;
	for(; idx > 0; idx-- ) {
		code = in_value[idx];
		bpos = bpos % 4;

		if (code < 0) {
			code = 256 + code;
		}
		res->bytes[vidx] |= (code << (bpos*8));

		if(bpos == 3) vidx++;
		bpos++;
	}
	res->length = vidx + 1;
	res->_is_field = true;
}

void FreeField(field_t * field) {
	//free(field->bytes);
	memset(field->bytes, 0x00, 22 * sizeof(uint32_t));
	field->length = 0;
}

void FieldFromUint32Buf(uint32_t * in_value, int len, field_t * res) {
	PORTD ^= 0x40;
	res->length = len;
	memset(&res->bytes[0], 0x00, 2 * sizeof(uint32_t));
	memcpy(&res->bytes[0], &in_value[0], res->length * sizeof(uint32_t));
	res->_is_field = true;
}

uint32_t FieldMod_Mul_s2[9] = { 0,0,0,0,0,0,0,0,0 };

void FieldMod_Mul(field_t * thisfield, field_t * thatfield, field_t * res) {
	PORTD ^= 0x40;
	uint32_t s[22];
	memcpy(&s[0], &Curve_mod_tmp[0], sizeof(uint32_t) * 22);
	g2fmfmul(thisfield->bytes, thisfield->length, thatfield->bytes, thatfield->length, s, 22);
	g2fmffmod(s, 22, Priv_mod_bits, 3, FieldMod_Mul_s2);
	FieldFromUint32Buf(FieldMod_Mul_s2, Priv_mod_words, res);
}

void FieldMod_Sqr(field_t * thisfield, field_t * res) {
	FieldMod_Mul(thisfield, thisfield, res);
}

void FieldAddM(field_t * thisfield, field_t * thatfield, uint32_t * _from, int fromlen) {
	PORTD ^= 0x40;
	uint32_t that_b[20];
	uint32_t this_b[20];
	uint32_t to_b[20];
	memcpy(&that_b, thatfield->bytes, thatfield->length * sizeof(uint32_t));
	if (fromlen > 0) {
		for (int i = 0; i < fromlen; i++)
		{
			this_b[i] = _from[i];
		}
	} else {
		for (int i = 0; i < thisfield->length; i++)
		{
			this_b[i] = thisfield->bytes[i];
		}
	}
	memcpy(&to_b, thisfield->bytes, thisfield->length * sizeof(uint32_t));
	int thatlen = thatfield->length;
	int to_b_len = thisfield->length;
	int iter_len = fromlen > thatlen ? 
	(fromlen > to_b_len ? fromlen : (to_b_len > thatlen ? to_b_len : thatlen)) :
	thatlen;

	if (to_b_len < thatlen) {
		memset(&to_b, 0x00, thisfield->length * 4);
		to_b_len = thisfield->length;
	}
	for(uint8_t i = 0; i < iter_len; i++) {
		to_b[i] = this_b[i] ^ ((i <= thatfield->length - 1) ? that_b[i] : 0);
	}

	memcpy(&thisfield->bytes[0], &to_b[0], to_b_len * sizeof(uint32_t));
	thisfield->length = to_b_len;
}

void FieldAdd(field_t * thisfield, field_t * thatfield, field_t * res) {
	PORTD ^= 0x40;
	FieldFromCurve(res);
	FieldAddM(res, thatfield, thisfield->bytes, thisfield->length);
}

bool FieldIs_Zero(field_t * field) {
	PORTD ^= 0x40;
	int blen = field->length;
	bool res = true;
	for(int idx = 0; idx < blen; idx++)
	{
		res &= (field->bytes[idx] == 0);
	}
	return res;
}

bool FieldEquals(field_t * thisfield, field_t * thatfield) {
	PORTD ^= 0x40;
	int blen = thisfield->length;
	int olen = thatfield->length;
	bool res = true;
	while(thatfield->bytes[olen-1] == 0 && olen > 0) olen--;
	while(thisfield->bytes[blen-1] == 0 && blen > 0) blen--;
	if (olen != blen) {
		return false;
	}

	for(uint8_t idx=0; idx < blen; idx++) {
		res &= ((thisfield->bytes[idx] - thatfield->bytes[idx]) == 0);
	}

	return res;
}

bool FieldIs_Less(field_t * thisfield, field_t * thatfield) {
	PORTD ^= 0x40;
	int blen = thisfield->length;
	int olen = thatfield->length;
	while(thatfield->bytes[olen - 1] == 0) olen--;
	while(thisfield->bytes[blen - 1] == 0) blen--;
	while (blen > 0 && (thisfield->bytes[blen - 1] - thatfield->bytes[blen - 1] == 0))
	{
		blen--;
	}
	return blen == 0 ? false : (thisfield->bytes[blen - 1] < thatfield->bytes[blen - 1]);
}

uint32_t FieldBitLength(field_t * field) {
	PORTD ^= 0x40;
	return g2fmblength(&field->bytes[0], field->length);
}

bool FieldTestBit(uint32_t n, field_t * field) {
	PORTD ^= 0x40;
	int test_word = floor(n / 32);
	int test_bit = n % 32;
	if (test_word < 0 || test_word > field->length) {
		return true;
	}
	uint32_t word = field->bytes[test_word];
	int mask = 1 << test_bit;

	return (word & mask) != 0;
}

void FieldClone(field_t * thatfield, field_t * res) {
	PORTD ^= 0x40;
	FieldFromUint32Buf(thatfield->bytes, thatfield->length, res);
}

void FieldClearBit(field_t * field, uint32_t n) {
	PORTD ^= 0x40;
	int test_word = floor(n / 32);
	int test_bit = n % 32;
	int mask = 1 << test_bit;
	if (field->length < test_word - 1)
	{
		return;
	}

	field->bytes[test_word] ^= (field->bytes[test_word] & mask);
}

void FieldSetBit(field_t * field, uint32_t n) {
	PORTD ^= 0x40;
	int test_word = floor(n / 32);
	int test_bit = n % 32;
	int mask = 1 << test_bit;
	if (field->length < test_word - 1)
	{
		return;
	}
	field->bytes[test_word] = mask;
}

void FieldShiftRight(field_t * field, uint32_t n, field_t * res) {
	PORTD ^= 0x40;
	if (n == 0) FieldClone(field, res);
	g2fmshiftRight(field->bytes, field->length, n);
	FieldFromUint32Buf(field->bytes, field->length, res);
}

void FieldShiftRightM(field_t * field, uint32_t n) {
	PORTD ^= 0x40;
	if (n == 0) return;
	g2fmshiftRight(field->bytes, field->length, n);
}

//uint8_t * FieldBuf8(field_t * field) {
	//uint8_t * ret = malloc(field->length * 4);
	//size_t len = field->length * sizeof(uint8_t);
//
	//for (uint8_t idx = 0; idx < field->length; idx++) {
		//ret[len - idx * sizeof(uint8_t) - 1] = field->bytes[idx] & 0xFF;
		//ret[len - idx * sizeof(uint8_t) - 2] = field->bytes[idx] >> 8 & 0xFF;
		//ret[len - idx * sizeof(uint8_t) - 3] = field->bytes[idx] >> 16 & 0xFF;
		//ret[len - idx * sizeof(uint8_t) - 4] = field->bytes[idx] >> 24 & 0xFF;
	//}
	//return ret;
//}
//
//uint8_t * FieldTruncate_Buf8(field_t * field) {
	//uint8_t * ret = FieldBuf8(field);
	//size_t start = field->length * 4 - g2fmblength(Curve_field_order, 17);
	//if(start < 0) {
		//return ret;
	//}
	//size_t newretlen = field->length * 4 - start;
	//uint8_t * newret = malloc(newretlen);
	//memcpy(newret, &ret[start], newretlen);
	//free(ret);
	//return newret;
//}

uint8_t FieldTrace(field_t * field) {
	PORTD ^= 0x40;
	uint32_t bitm_l = Curve_m;
	field_t * rv = malloc(sizeof(field_t));
	FieldClone(field, rv);
	for (uint8_t idx = 1; idx <= bitm_l - 1; idx++) {
		FieldMod_Mul(field, rv, rv);
		FieldAddM(rv, field, 0, 0);
	}
	return rv->bytes[0] & 1;
	free(rv);
}

void FieldInvert(field_t * field, field_t * res) {
	PORTD ^= 0x40;
	uint32_t p[9];
	uint32_t a[10];
	uint32_t tmpres[10];
	g2fmffmod(&field->bytes[0], field->length, Priv_mod_bits, 3, a);
	CurveCalc_Modulus(p);
	g2fmfinv(a, 9, p, Priv_mod_words, tmpres);
	FieldFromUint32Buf(tmpres, field->length, res);
}

void FieldTruncate(field_t * field, field_t * res) {
	PORTD ^= 0x40;
	field_t tmporderfield;
	FieldFromUint32Buf(Curve_field_order, 17, &tmporderfield);
	uint32_t bitl_o = FieldBitLength(&tmporderfield);
	uint32_t xbit = FieldBitLength(field) + 1;
	memcpy(res, field, sizeof(field_t));
	while (bitl_o <= xbit) {
		FieldClearBit(res, xbit - 1);
		xbit = FieldBitLength(res);
	}
	FreeField(&tmporderfield);
}

void FieldCreateRandom(field_t * res) {
	PORTD ^= 0x40;
	field_t * tmpf = malloc(sizeof(field_t));
	FieldFromUint32Buf(Curve_field_order, 17, tmpf);
	size_t bits = FieldBitLength(tmpf);
	uint8_t words = bits / 8;
	uint8_t * rand8 = malloc(words);
	field_t ret;
	while (1) {
		for (uint8_t i = 0; i < words; i++)
		{
			rand8[i] = GetRandomByte();
		}
		FieldFromByteArray(rand8, words, words, &ret);
		if (FieldIs_Less(&ret, tmpf))
		{
			break;
		}
	}
	res->length = ret.length;
	memcpy(&res->bytes[0], &ret.bytes[0], sizeof(uint32_t) * ret.length);
	free(rand8);
	free(tmpf);
}

field_t * FieldCompress(point_t * point) {
	PORTD ^= 0x40;
	field_t * x_inv = malloc(sizeof(field_t));
	FieldInvert(&point->x, x_inv);
	field_t * tmp = malloc(sizeof(field_t));
	FieldMod_Mul(x_inv, &point->y, tmp);
	uint8_t trace = FieldTrace(tmp);
	if (trace == 1) {
		FieldSetBit(&point->x, 0);
		return &point->x;
	}
	FieldClearBit(&point->x, 0);
	return &point->x;
}