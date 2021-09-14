/*
 * Curve.c
 *
 * Created: 04.09.2021 17:52:48
 *  Author: root
 */ 

#include "Curve.h"
#include "gf2m.h"
#include "wnaf.h"
#include "wnafmul.h"
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

uint32_t * CurveCalc_Modulus(curve_t * curve) {
	uint32_t * ret = malloc(curve->mod_words * 4);
	ret[0] = 1;

	uint32_t word = floor(curve->m / 32);
	uint32_t bit = curve->m % 32;
	ret[word] |= 1 << bit;

	for (uint8_t i = 0; i < curve->ks_len; i += 1) {
		word = floor(curve->ks[i] / 32);
		bit = curve->ks[i] % 32;
		ret[word] |= 1 << bit;
	}

	return ret;
}

void CurveExpandXtoY(curve_t * curve, field_t * x, field_t * expanded_x, field_t * expanded_y) {
	if (FieldIs_Zero(x)) {
		expanded_y = FieldMod_Mul(curve->b, curve->b, curve);
	}
	memcpy(expanded_x, x, sizeof(field_t));
	bool k = FieldTestBit(0, expanded_x);
	expanded_x = FieldClearBit(expanded_x, 0, curve);
	uint8_t trace = FieldTrace(expanded_x, curve);
	if ((trace != 0 && FieldIs_Zero(curve->a)) || (trace == 0 && FieldEquals(curve->a, curve->one))) {
		expanded_x = FieldSetBit(expanded_x, 0, curve);
	}
	field_t * x2 = FieldMod_Mul(expanded_x, expanded_x, curve);
	expanded_y = FieldMod_Mul(x2, expanded_x, curve);
	if (FieldEquals(curve->a, curve->one)) {
		FieldAddM(expanded_y, x2, 0, 0);
	}
	FieldAddM(expanded_y, curve->a, 0, 0);
	field_t * invx2 = FieldInvert(x2, curve);
	expanded_y = FieldMod_Mul(expanded_y, invx2, curve);
	expanded_y = Curvefsquad(expanded_y, curve);
	uint8_t trace_y = FieldTrace(expanded_y, curve);

	if ((k && trace_y == 0) || (!k && trace_y != 0)) {
		expanded_y->bytes[0] ^= 1;
	}
	expanded_y = FieldMod_Mul(expanded_y, x, curve);
}

field_t * Curvefsquad(field_t * field, curve_t * curve) {
	
	field_t * ret = Curvefsquad_odd(field, curve);

	g2fmffmod(ret->bytes, ret->length, curve->mod_bits, curve->mod_bits_len, ret->bytes);
	return ret;
}

field_t * Curvefsquad_odd(field_t * field, curve_t * curve) {
	uint32_t bitl_m = curve->m;
	uint32_t range_to = (bitl_m - 1) / 2;
	field_t * val_a = malloc(sizeof(field_t));
	g2fmffmod(field->bytes, field->length, curve->mod_bits, curve->mod_bits_len, val_a->bytes);

	field_t * val_z = malloc(sizeof(field_t));
	memcpy(val_z, val_a, sizeof(field_t));

	for (size_t idx = 1; idx <= range_to; idx += 1) {
		val_z = FieldMod_Sqr(val_z, curve);
		val_z = FieldMod_Sqr(val_z, curve);
		g2fmffmod(val_a->bytes, val_a->length, curve->mod_bits, curve->mod_bits_len, val_z->bytes);
	}

	field_t * val_w = FieldMod_Mul(val_z, val_z, curve);
	FieldAddM(val_w, val_z, 0, 0);

	if (FieldEquals(val_w, val_a)) {
		return val_z;
	}

	return 0;
}

bool CurveContainsPoint(curve_t * curve, point_t * point) {
	field_t * lh = FieldAdd(point->x, curve);
	lh = FieldMod_Mul(lh, point->x, curve);
	FieldAddM(lh, point->y, 0, 0);
	lh = FieldMod_Mul(lh, point->x, curve);
	FieldAddM(lh, curve->b, 0, 0);
	field_t * y2 = FieldMod_Mul(point->y, point->y, curve);
	FieldAddM(lh, y2, 0, 0);
	return FieldIs_Zero(lh);
}

void CurveSetBase(field_t * base_x, field_t * base_y, curve_t * curve) {
	size_t width = getWindowSize(curve->m);
	width = width < 16 ?
	width > 2 ? width : 2
	: 16;
	curve->Base = PointConstructor(curve, base_x, base_y);
	PointPrecomp(curve->Base, curve, width);
	//field_t 8 cmp = FieldCompress(curve->Base, curve);
}

field_t * CurveCompModulus(uint32_t m, uint32_t * ks, size_t ks_len, curve_t * curve) {
	field_t * modulus = malloc(sizeof(field_t));
	memcpy(modulus, &curve->one, sizeof(field_t));
	FieldSetBit(modulus, m, curve);
	for (size_t i = 0; i < ks_len; i++)
	{
		FieldSetBit(modulus, ks[i], curve);
	}
	return modulus;
}