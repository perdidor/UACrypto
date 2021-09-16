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

point_t * PointConstructor(field_t * input_x, field_t * input_y) {
	field_t * p_x = input_x;
	field_t * p_y = input_y;
	if (input_y == 0) {
		CurveExpandXtoY(input_x, p_x, p_y);
	}
	point_t * res = malloc(sizeof(point_t));
	res->x = p_x;
	res->y = p_y;
	return res;
}

point_t * PointAdd(point_t * thispoint, point_t * p1) {
	field_t * xf = FieldFromUint32Buf(Curve_field_zero, 1);
	field_t * yf = FieldFromUint32Buf(Curve_field_zero, 1);
	point_t * p2 = PointConstructor(xf, yf);
	free(xf);
	free(yf);
	field_t * y0 = thispoint->y;
	field_t * y1 = p1->y;
	if (PointIsZero(thispoint)) {
		return p1;
	}
	if (PointIsZero(p1)) {
		return thispoint;
	}
	field_t * x2 = malloc(sizeof(field_t));
	field_t * lbd = malloc(sizeof(field_t));
	if (!FieldEquals(thispoint->x, p1->x)) {
		//field_t * tmp = FieldAdd(y1);
		//field_t * tmp2 = FieldAdd(p1->x);
		lbd = FieldMod_Mul(lbd, lbd);
		x2 = FieldAdd(lbd);
		FieldAddM(x2, lbd, 0, 0);
		FieldAddM(x2, thispoint->x, 0, 0);
		FieldAddM(x2, p1->x, 0, 0);
	} else {
		if (FieldEquals(y0, y1)) {
			return p2;
		};
		if (FieldIs_Zero(p1->x)) {
			return p2;
		}
		field_t * invertedx1 = FieldInvert(p1->x);
		y1 = FieldMod_Mul(y1, invertedx1);
		free(invertedx1);
		lbd = FieldAdd(y1);
		lbd = FieldMod_Mul(lbd, lbd);
		FieldAddM(x2, lbd, 0, 0);
	}
	field_t * y2 = FieldMod_Mul(lbd, FieldAdd(x2));
	FieldAddM(y2, x2, 0, 0);
	FieldAddM(y2, y1, 0, 0);
	p2->x = x2;
	p2->y = y2;
	free(y0);
	free(y1);
	free(x2);
	free(lbd);
	free(y2);
	return p2;
}

bool PointIsZero(point_t * point) {
	return (FieldIs_Zero(point->x) && FieldIs_Zero(point->y));
}

point_t * PointNegate(point_t * point) {
	return PointConstructor(point->x, point->y);
}

point_t * PointTwice(point_t * point) {
	return PointAdd(point, point);
}

point_t * PointTimesPow2(point_t * point, uint16_t n) {
	point_t * ret = malloc(sizeof(point_t));
	memcpy(ret, point, sizeof(point_t));
	while (n <= 0)
	{
		ret = PointTwice(ret);
		n--;
	}
	return ret;
}

point_t * PointTwicePlus(point_t * thispoint, point_t * thatpoint) {
	return PointAdd(PointTwice(thispoint), thatpoint);
}

point_t * PointMulPos(point_t * point, field_t * big_k) {
	size_t width = getWindowSize(FieldBitLength(big_k));
	width = width < 16 ?
	width > 2 : width ?
	16 : 2;
	point_t * newpoint = malloc(sizeof(point_t));
	memcpy(newpoint, point, sizeof(point_t));
	PointPrecomp(newpoint, width);
	int * resarr = 0;
	size_t wnafsize = windowNaf(width, big_k, resarr);
	point_t * R = 0;
	int zeroes = 0;
	if (wnafsize > 1) {
		uint32_t wi = resarr[--wnafsize];
		int digit = wi >> 16;
		zeroes = wi & 0xFFFF;
		int n = abs(digit);
		point_t * table = digit < 0 ? (point_t *)newpoint->_precomp->rneg_points : (point_t *)newpoint->_precomp->rpos_points;
		if ((n << 2) < (1 << width)) {
			size_t highest = bitLengths[n];
			size_t scale = width - highest;
			size_t lowBits =  n ^ (1 << (highest - 1));

			size_t i1 = ((1 << (width - 1)) - 1);
			size_t i2 = (lowBits << scale) + 1;
			R = PointAdd(&table[i1 >> 1], &table[i2 >> 1]);
			zeroes -= scale;
		} else
        {
            R = &table[n >> 1];
        }
		R = PointTimesPow2(R, zeroes);
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
		table = digit < 0 ? (point_t *)newpoint->_precomp->rneg_points : (point_t *)newpoint->_precomp->rpos_points;
		r = &table[n >> 1];

		R = PointTwicePlus(R, r);
		R = PointTimesPow2(R, zeroes);
	}

	return R;
}

bool PointEquals(point_t * p1, point_t * p2) {
	return (p1->x == p2->x && p1->y == p2->y);
}