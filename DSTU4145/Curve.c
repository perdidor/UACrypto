/*
 * .c
 *
 * Created: 04.09.2021 17:52:48
 *  Author: root
 */ 

#include "Curve.h"
#include "gf2m.h"
#include "wnaf.h"
#include "wnafmul.h"
#include "ConvertHelper.h"
#include "PrivateKey.h"
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

//uint32_t * CurveCalc_Modulus() {
	//uint32_t * ret = malloc(Priv_mod_words * 4);
	//ret[0] = 1;
//
	//uint32_t word = floor(Curve_m / 32);
	//uint32_t bit = Curve_m % 32;
	//ret[word] |= 1 << bit;
//
	//for (uint8_t i = 0; i < ks_len; i += 1) {
		//word = floor(ks[i] / 32);
		//bit = ks[i] % 32;
		//ret[word] |= 1 << bit;
	//}
//
	//return ret;
//}

//void CurveExpandXtoY(field_t * x, field_t * expanded_x, field_t * expanded_y) {
	//if (FieldIs_Zero(x)) {
		//expanded_y = FieldMod_Mul(b->b);
	//}
	//memcpy(expanded_x, x, sizeof(field_t));
	//bool k = FieldTestBit(0, expanded_x);
	//expanded_x = FieldClearBit(expanded_x, 0);
	//uint8_t trace = FieldTrace(expanded_x);
	//if ((trace != 0 && FieldIs_Zero(a)) || (trace == 0 && FieldEquals(a->one))) {
		//expanded_x = FieldSetBit(expanded_x, 0);
	//}
	//field_t * x2 = FieldMod_Mul(expanded_x, expanded_x);
	//expanded_y = FieldMod_Mul(x2, expanded_x);
	//if (FieldEquals(a->one)) {
		//FieldAddM(expanded_y, x2, 0, 0);
	//}
	//FieldAddM(expanded_y->a, 0, 0);
	//field_t * invx2 = FieldInvert(x2);
	//expanded_y = FieldMod_Mul(expanded_y, invx2);
	//expanded_y = Curvefsquad(expanded_y);
	//uint8_t trace_y = FieldTrace(expanded_y);
//
	//if ((k && trace_y == 0) || (!k && trace_y != 0)) {
		//expanded_y->bytes[0] ^= 1;
	//}
	//expanded_y = FieldMod_Mul(expanded_y, x);
//}

//field_t * Curvefsquad(field_t * field) {
	//
	//field_t * ret = Curvefsquad_odd(field);
//
	//g2fmffmod(ret->bytes, 3, ret->bytes);
	//return ret;
//}

//field_t * Curvefsquad_odd(field_t * field) {
	//uint32_t bitl_m = Curve_m;
	//uint32_t range_to = (bitl_m - 1) / 2;
	//field_t * val_a = malloc(sizeof(field_t));
	//g2fmffmod(field->bytes, 3, val_a->bytes);
//
	//field_t * val_z = malloc(sizeof(field_t));
	//memcpy(val_z, val_a, sizeof(field_t));
//
	//for (size_t idx = 1; idx <= range_to; idx += 1) {
		//val_z = FieldMod_Sqr(val_z);
		//val_z = FieldMod_Sqr(val_z);
		//g2fmffmod(val_a->bytes, 3, val_z->bytes);
	//}
//
	//field_t * val_w = FieldMod_Mul(val_z, val_z);
	//FieldAddM(val_w, val_z, 0, 0);
//
	//if (FieldEquals(val_w, val_a)) {
		//return val_z;
	//}
//
	//return 0;
//}

//bool CurveContainsPoint(point_t * point) {
	//field_t * lh = FieldAdd(point->x);
	//lh = FieldMod_Mul(lh, point->x);
	//FieldAddM(lh, point->y, 0, 0);
	//lh = FieldMod_Mul(lh, point->x);
	//FieldAddM(lh->bytes, 0, 0);
	//field_t * y2 = FieldMod_Mul(point->y, point->y);
	//FieldAddM(lh, y2, 0, 0);
	//return FieldIs_Zero(lh);
//}

//void CurveSetBase(field_t * base_x, field_t * base_y) {
	//size_t width = getWindowSize(Curve_m);
	//width = width < 16 ?
	//width > 2 ? width : 2
	//: 16;
	//Base = PointConstructor(base_x, base_y);
	//PointPrecomp(Base, width);
	////field_t 8 cmp = FieldCompress(Base);
//}

field_t * CurveCompModulus(uint32_t Curve_m, uint32_t * ks, size_t ks_len) {
	field_t * modulus = malloc(sizeof(uint32_t) * 9);
	memcpy(modulus, &Curve_field_one[0], sizeof(uint32_t) * 9);
	FieldSetBit(modulus, Curve_m);
	for (size_t i = 0; i < ks_len; i++)
	{
		FieldSetBit(modulus, ks[i]);
	}
	return modulus;
}