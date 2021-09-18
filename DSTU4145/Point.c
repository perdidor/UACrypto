/*
 * Point.c
 *
 * Created: 08.09.2021 16:22:48
 *  Author: root
 */ 
#include "Point.h"
#include "Field.h"
#include "wnaf.h"
#include "Curve.h"
#include "dstu_types.h"
#include "wnafmul.h"
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
#include "PrivateKey.h"

uint32_t dummycar;

point_t * PointConstructor(field_t * input_x, field_t * input_y) {
	field_t * p_x = input_x;
	field_t * p_y = input_y;
	if (input_y == 0) {
		CurveExpandXtoY(input_x, p_x, p_y);
	}
	point_t * res = malloc(sizeof(point_t));
	memcpy(&res->x, p_x, sizeof(field_t));
	memcpy(&res->y, p_y, sizeof(field_t));
	free(p_x);
	free(p_y);
	return res;
}

uint32_t suka(field_t * asd) {
	uint32_t asdd = asd->bytes[1];
	if (asdd == 21) {
		dummycar = asdd + 232;
	}
	return asdd;
}

void PointAdd(point_t * thispoint, point_t * p1, point_t * res) {
	if (PointIsZero(thispoint)) {
		memcpy(res, p1, sizeof(point_t));
		return;
	}
	if (PointIsZero(p1)) {
		memcpy(res, thispoint, sizeof(point_t));
		return;
	}
	field_t param_a, tmp, tmp2, invertedtmp, lbdtmp, y0, y1, xf, yf;
	uint32_t asd = 0, asd1 = 1;
	FieldFromUint32Buf(Curve_field_zero, 1, &xf);
	FieldFromUint32Buf(Curve_field_zero, 1, &yf);
	point_t * p2 = PointConstructor(&xf, &yf);
	
	//memcpy(&y0, &thispoint->y, sizeof(field_t));
	//memcpy(&y1, &p1->y, sizeof(field_t));
	y0.length = thispoint->y.length;
	y0.bytes = malloc(thispoint->y.length * sizeof(uint32_t));
	for (int i = 0; i < thispoint->y.length; i++)
	{
		y0.bytes[i] = thispoint->y.bytes[i];
	}
	y1.length = p1->y.length;
	for (int i = 0; i < p1->y.length; i++)
	{
		y1.bytes[i] = p1->y.bytes[i];
	}
	static uint32_t x0bytes[10];
	static uint32_t x1bytes[10];
	static field_t lbd, x2, x0, x1, tmplbd;
	for (int i = 0; i < thispoint->x.length; i++)
	{
		x0bytes[i] = thispoint->x.bytes[i];
		x1bytes[i] = p1->x.bytes[i];
	}
	static uint32_t lbdbytes[10];
	FieldFromUint32Buf(x1bytes, 10, &x1);
	FieldFromUint32Buf(Curve_field_a, 10, &param_a);
	if (!FieldEquals(&thispoint->x, &p1->x)) {
		static uint32_t tmpint32 = 0;
		static field_t x2;
		FieldAdd(&y0, &y1, &tmp);
		FieldAdd(&thispoint->x, &p1->x, &tmp2);
		FieldInvert(&tmp2, &invertedtmp);
		FieldMod_Mul(&tmp, &invertedtmp, &lbd);
		for (int i = 0; i < lbd.length; i++)
		{
			lbdbytes[i] = lbd.bytes[i];
		}
		FieldMod_Mul(&lbd, &lbd, &lbdtmp);

		FieldFromUint32Buf(lbdbytes, 9, &tmplbd);

		FieldAdd(&param_a, &lbdtmp, &x2);

		FieldAddM(&x2, &tmplbd, 0, 0);
		tmpint32 = suka(&x2);
		FieldFromUint32Buf(x0bytes, 10, &x0);
		FieldAddM(&x2, &x0, 0, 0);

		FieldAddM(&x2, &p1->x, 0, 0);
	} else {
		if (!FieldEquals(&y0, &y1) || FieldIs_Zero(&p1->x)) {
			memcpy(res, p2, sizeof(point_t));
			return;
		};
		field_t invertedx1, lbdtmp;
		FieldInvert(&p1->x, &invertedx1);
		FieldMod_Mul(&y1, &invertedx1, &y1);
		FieldAdd(&p1->x, &y1, &lbd);
		FieldMod_Mul(&lbd, &lbd, &lbdtmp);
		FieldAdd(&lbdtmp, &param_a, &x2);
		FieldAddM(&x2, &lbdtmp, 0, 0);
	}
	field_t y2, tmpx;
	FieldFromUint32Buf(lbdbytes, 9, &lbd);
	FieldAdd(&p1->x, &x2, &tmpx);
	FieldMod_Mul(&lbd, &tmpx, &y2);
	FieldAddM(&y2, &x2, 0, 0);
	FieldAddM(&y2, &y1, 0, 0);
	memcpy(&res->x, &x2, x2.length);
	memcpy(&res->y, &y2, y2.length);
	res->x.length = x2.length;
	res->y.length = y2.length;
	//free(xf);
	//free(yf);
	//free(y0);
	//free(y1);
	//free(x2);
	asd = asd1;
}

bool PointIsZero(point_t * point) {
	return (FieldIs_Zero(&point->x) && FieldIs_Zero(&point->y));
}

point_t * PointNegate(point_t * point) {
	return PointConstructor(&point->x, &point->y);
}

void PointTwice(point_t * point, point_t * res) {
	PointAdd(point, point, res);
}

point_t * PointTimesPow2(point_t * point, uint16_t n) {
	point_t * ret = malloc(sizeof(point_t));
	memcpy(ret, point, sizeof(point_t));
	while (n <= 0)
	{
		PointTwice(ret, ret);
		n--;
	}
	return ret;
}

void PointTwicePlus(point_t * thispoint, point_t * thatpoint, point_t * res) {
	point_t * tmp = malloc(sizeof(point_t));
	PointTwice(thispoint, tmp);
	PointAdd(tmp, thatpoint, res);
	free(tmp);
}

void PointMulPos(point_t * point, field_t * big_k, point_t * res) {
	uint32_t width = getWindowSize(FieldBitLength(big_k));
	width = width < 16 ?
	(width > 2 ? width : 16) : 2;
	point_t * newpoint = malloc(sizeof(point_t));
	memcpy(newpoint, point, sizeof(point_t));
	//PointPrecomp(newpoint, width);
	int32_t resarr[51];
	uint32_t wnafsize = windowNaf(width, big_k, resarr);
	point_t R;
	int zeroes = 0;
	static uint32_t tmpint32 = 0;
	if (wnafsize > 1) {
		uint32_t wi = resarr[--wnafsize];
		int digit = wi >> 16;
		zeroes = wi & 0xFFFF;
		int n = abs(digit);
		point_precomputed_t * table = malloc(sizeof(point_precomputed_t) * 8);
		if (digit < 0) {
			memcpy(table, newpoint->precomp_neg, sizeof(point_precomputed_t) * 8);
		} else {
			memcpy(table, newpoint->precomp_pos, sizeof(point_precomputed_t) * 8);
		}
		if ((n << 2) < (1 << width)) {
			size_t highest = bitLengths[n];
			size_t scale = width - highest;
			size_t lowBits =  n ^ (1 << (highest - 1));

			size_t i1 = ((1 << (width - 1)) - 1);
			size_t i2 = (lowBits << scale) + 1;
			point_t tablei1;
			point_t tablei2;
			FieldFromUint32Buf(table[i1 >> 1].x.bytes, table[i1 >> 1].x.length, &tablei1.x);
			FieldFromUint32Buf(table[i1 >> 1].y.bytes, table[i1 >> 1].y.length, &tablei1.y);
			FieldFromUint32Buf(table[i2 >> 1].x.bytes, table[i2 >> 1].x.length, &tablei2.x);
			FieldFromUint32Buf(table[i2 >> 1].y.bytes, table[i2 >> 1].y.length, &tablei2.y);
			PointAdd(&tablei1, &tablei2, &R);
			zeroes -= scale;
		} else
        {
			memcpy(&R.x, &table[(n >> 1)].x, sizeof(field_t));
			memcpy(&R.y, &table[(n >> 1)].y, sizeof(field_t));
        }
		point_t * tmp = PointTimesPow2(&R, zeroes);
		memcpy(&R, tmp, sizeof(point_t));
		tmpint32 = suka(&R.y);
	}

	uint32_t wi = 0;
	int digit = 0;
	int n = 0;
	point_t * table = 0;
	point_t * r = 0;
	while (wnafsize > 0)
	{
		wi = resarr[--wnafsize];
		digit = wi >> 16;
		zeroes = wi & 0xFFFF;

		n = abs(digit);
		//table = digit < 0 ? (point_t *)newpoint->_precomp->rneg_points : (point_t *)newpoint->_precomp->rpos_points;
		r = &table[n >> 1];

		PointTwicePlus(&R, r, &R);
		point_t * tmp = PointTimesPow2(&R, zeroes);
		memcpy(&R, tmp, sizeof(point_t));
	}

	//return R;
}

bool PointEquals(point_t * p1, point_t * p2) {
	return (&p1->x == &p2->x && &p1->y == &p2->y);
}