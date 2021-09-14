/*
 * Field.c
 *
 * Created: 04.09.2021 17:56:36
 *  Author: root
 */ 
#include "Curve.h"
#include "gf2m.h"
#include "dstu_types.h"
#include "Field.h"
#include "ConvertHelper.h"
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

void FieldSetCommonValues(curve_t * curve, field_t * field) {
	field->_is_field = true;
	field->mod_bits = curve->mod_bits;
	field->mod_bits_len = curve->mod_bits_len;
	field->mod_words = curve->mod_words;
}

field_t * FieldFromCurve(curve_t * curve) {
	field_t * res = malloc(sizeof(field_t));
	res->bytes = malloc(curve->mod_words * sizeof(uint32_t));
	res->length = curve->mod_words;
	FieldSetCommonValues(curve, res);
	return res;
}

field_t * FieldFromHexStr(char * in_value, curve_t * curve) {
	field_t * res = malloc(sizeof(field_t));
	res->length = ConvertHex2Bin(in_value, res->bytes);
	FieldSetCommonValues(curve, res);
	return res;
}

field_t * FieldFromByteArray(uint8_t * in_value, size_t len, size_t max_size, curve_t * curve) {
	field_t * res = malloc(sizeof(field_t));
	size_t tmpsize = ceil(len / 4);
	tmpsize = (tmpsize > max_size ? tmpsize : max_size);
	res->bytes = malloc(curve->mod_words * tmpsize);
	uint8_t code = 0;
	uint8_t bpos = 0;
	uint8_t vidx = 0;
	for(uint8_t idx = len-1; idx >= 0; idx-- ) {
		code = in_value[idx];
		bpos = bpos % 4;

		if (code < 0) {
			code = 256 + code;
		}
		res->bytes[vidx] |= code << (bpos*8);

		if(bpos == 3) vidx++;
		bpos++;
	}
	FieldSetCommonValues(curve, res);
	return res;
}

field_t * FieldFromUint32Buf(uint32_t * in_value, size_t len, curve_t * curve) {
	field_t * res = malloc(sizeof(field_t));
	res->bytes = in_value;
	res->length = len;
	FieldSetCommonValues(curve, res);
	return res;
}

field_t * FieldMod_Mul(field_t * thisfield, field_t * thatfield, curve_t * curve) {
	uint32_t * s = curve->mod_tmp;
	size_t s_len = curve->mod_bits_len;
	g2fmfmul(thisfield->bytes, thisfield->length, thatfield->bytes, thatfield->length, s, s_len);
	uint32_t * s2 = malloc(s_len * sizeof(uint8_t));
	g2fmffmod(s, s_len, thisfield->mod_bits, thisfield->mod_bits_len, s2);
	return FieldFromUint32Buf(s2, s_len, curve);
}

field_t * FieldMod_Sqr(field_t * thisfield, curve_t * curve) {
	return FieldMod_Mul(thisfield, thisfield, curve);
}

void FieldAddM(field_t * thisfield, field_t * thatfield, uint32_t * _from, size_t fromlen) {
	uint32_t * that_b = thatfield->bytes;
	size_t thatlen = thatfield->length;
	uint32_t * this_b = fromlen > 0 ? _from : thisfield->bytes;
	uint32_t * to_b = thisfield->bytes;
	size_t to_b_len = thisfield->length;
	size_t iter_len = fromlen > thatlen ? 
	(fromlen > to_b_len ? fromlen : (to_b_len > thatlen ? to_b_len : thatlen)) :
	thatlen;

	if (to_b_len < thatlen) {
		to_b = malloc(thisfield->mod_words * 4);
		to_b_len = thisfield->mod_words;
	}

	for(uint8_t i = 0; i < iter_len; i++) {
		to_b[i] = this_b[i] ^ ((i <= thatfield->length - 1) ? that_b[i] : 0);
	}

	thisfield->bytes = to_b;
	thisfield->length = to_b_len;
}

field_t * FieldAdd(field_t * thatfield, curve_t * curve) {
	field_t * res = FieldFromCurve(curve);
	FieldAddM(res, thatfield, 0, 0);
	return res;
}

bool FieldIs_Zero(field_t * field) {
	size_t blen = field->length;
	for(uint8_t idx =0; idx<blen; idx++) {
		if(field->bytes[idx] != 0)
		return false;
	}
	return true;
}

bool FieldEquals(field_t * thisfield, field_t * thatfield) {
	size_t blen = thisfield->length;
	size_t olen = thatfield->length;
	uint32_t diff = 0;
	while(thatfield->bytes[olen-1] == 0) olen--;
	while(thisfield->bytes[blen-1] == 0) olen--;
	if (olen != blen) {
		return false;
	}

	for(uint8_t idx=0; idx<blen; idx++) {
		diff |= thisfield->bytes[idx] ^ thatfield->bytes[idx];
	}

	return diff == 0;
}

bool FieldIs_Less(field_t * thisfield, field_t * thatfield) {
	size_t blen = thisfield->length;
	size_t olen = thatfield->length;
	while(thatfield->bytes[olen-1] == 0) olen--;
	while(thisfield->bytes[blen-1] == 0) olen--;
	return thisfield->bytes[blen] < thatfield->bytes[olen];
}

size_t FieldBitLength(field_t * field) {
	return g2fmblength(field->bytes, field->length);
}

bool FieldTestBit(uint32_t n, field_t * field) {
	int test_word = floor(n / 32);
	int test_bit = n % 32;
	if (test_word < 0 || test_word > field->length) {
		return true;
	}
	uint32_t word = field->bytes[test_word];
	uint32_t mask = 1 << test_bit;

	return (word & mask) != 0;
}

field_t * FieldClone(field_t * thatfield, curve_t * curve) {
	return FieldFromUint32Buf(thatfield->bytes, thatfield->length, curve);
}

field_t * FieldClearBit(field_t * field, uint32_t n, curve_t * curve) {
	uint32_t test_word = floor(n / 32);
	uint32_t test_bit = n % 32;
	uint32_t mask = 1 << test_bit;
	if (field->length < test_word - 1)
	{
		return field;
	}

	uint32_t word = field->bytes[test_word];
	word ^= word & mask;
	field_t * newfield = FieldClone(field, curve);
	newfield->bytes[test_word] = word;
	return newfield;
}

field_t * FieldSetBit(field_t * field, uint32_t n, curve_t * curve) {
	uint32_t test_word = floor(n / 32);
	uint32_t test_bit = n % 32;
	uint32_t mask = 1 << test_bit;
	if (field->length < test_word - 1)
	{
		return field;
	}

	field_t * newfield = FieldClone(field, curve);
	newfield->bytes[test_word] = mask;
	return newfield;
}

field_t * FieldShiftRight(field_t * field, uint32_t n, curve_t * curve) {
	if (n == 0) return FieldClone(field, curve);
	return FieldFromUint32Buf(g2fmshiftRight(field->bytes, field->length, n, true), field->length, curve);
}

void FieldShiftRightM(field_t * field, uint32_t n) {
	if (n == 0) return;
	g2fmshiftRight(field->bytes, field->length, n, true);
}

uint8_t * FieldBuf8(field_t * field) {
	uint8_t * ret = malloc(field->length * 4);
	size_t len = field->length * 4;

	for (uint8_t idx = 0; idx < field->length; idx++) {
		ret[len - idx * 4 - 1] = field->bytes[idx] & 0xFF;
		ret[len - idx * 4 - 2] = field->bytes[idx] >> 8 & 0xFF;
		ret[len - idx * 4 - 3] = field->bytes[idx] >> 16 & 0xFF;
		ret[len - idx * 4 - 4] = field->bytes[idx] >> 24 & 0xFF;
	}
	return ret;
}

uint8_t * FieldTruncate_Buf8(field_t * field, curve_t * curve) {
	uint8_t * ret = FieldBuf8(field);
	size_t start = field->length * 4 - g2fmblength(curve->order->bytes, curve->order->length);
	if(start < 0) {
		return ret;
	}
	size_t newretlen = field->length * 4 - start;
	uint8_t * newret = malloc(newretlen);
	memcpy(newret, &ret[start], newretlen);
	return newret;
}

uint8_t FieldTrace(field_t * field, curve_t * curve) {
	uint32_t bitm_l = curve->m;
	field_t * rv = FieldClone(field, curve);
	for (uint8_t idx = 1; idx <= bitm_l - 1; idx++) {
		rv = FieldMod_Mul(field, rv, curve);
		FieldAddM(rv, field, 0, 0);
	}
	return rv->bytes[0] & 1;
}

field_t * FieldInvert(field_t * field, curve_t * curve) {
	uint32_t * a = malloc(field->length * 4);
	g2fmffmod(field->bytes, field->length, field->mod_bits, field->mod_bits_len, a);
	uint32_t * p = CurveCalc_Modulus(curve);
	size_t p_len = curve->mod_words * 4;
	g2fmfinv(a, field->length, p, p_len, a);
	return FieldFromUint32Buf(a, field->length, curve);
}

field_t * FieldTruncate(field_t * field, curve_t * curve) {
	size_t bitl_o = FieldBitLength(curve->order);
	size_t xbit = FieldBitLength(field);
	field_t * ret = malloc(sizeof(field_t));
	memcpy(ret, field, sizeof(field_t));
	while (bitl_o <= xbit) {
		ret = FieldClearBit(ret, xbit - 1, curve);
		xbit = FieldBitLength(ret);
	}
	return ret;
}

uint8_t CurveGetRandomByte() {
	return 0xfe;
}

field_t * FieldCreateRandom(curve_t * curve) {
	size_t bits = FieldBitLength(curve->order);
	size_t words = ceil(bits / 8);
	field_t * ret;
	while (1) {
		uint8_t * rand8 = malloc(words);
		for (uint8_t i = 0; i < words; i++)
		{
			rand8[i] = CurveGetRandomByte();
		}
		ret = FieldFromByteArray(rand8, words, words, curve);
		if (!FieldIs_Less(curve->order, ret))
		{
			break;
		}
	}
	return ret;
}

field_t * FieldCompress(point_t * point, curve_t * curve) {
	field_t * x_inv = FieldInvert(point->x, curve);
	field_t * tmp = FieldMod_Mul(x_inv, point->y, curve);
	uint8_t trace = FieldTrace(tmp, curve);
	if (trace == 1) {
		return FieldSetBit(point->x, 0, curve);
	}
	return FieldClearBit(point->x, 0, curve);
}