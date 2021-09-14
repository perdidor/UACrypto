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

point_t * PointConstructor(curve_t * curve, field_t * input_x, field_t * input_y) {
	field_t * p_x = input_x;
	field_t * p_y = input_y;
	if (input_y == 0) {
		CurveExpandXtoY(curve, input_x, p_x, p_y);
	}
	point_t * res = malloc(sizeof(point_t));
	res->x = p_x;
	res->y = p_y;
	return res;
}

point_t * PointAdd(point_t * thispoint, point_t * p1, curve_t * curve) {
	point_t * p2 = PointConstructor(curve, curve->zero, curve->zero);
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
		//field_t * tmp = FieldAdd(y1, curve);
		//field_t * tmp2 = FieldAdd(p1->x, curve);
		lbd = FieldMod_Mul(lbd, lbd, curve);
		x2 = FieldAdd(lbd, curve);
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
		field_t * invertedx1 = FieldInvert(p1->x, curve);
		y1 = FieldMod_Mul(y1, invertedx1, curve);
		lbd = FieldAdd(y1, curve);
		lbd = FieldMod_Mul(lbd, lbd, curve);
		FieldAddM(x2, lbd, 0, 0);
	}
	field_t * y2 = FieldMod_Mul(lbd, FieldAdd(x2, curve), curve);
	FieldAddM(y2, x2, 0, 0);
	FieldAddM(y2, y1, 0, 0);
	p2->x = x2;
	p2->y = y2;
	return p2;
}

bool PointIsZero(point_t * point) {
	return (FieldIs_Zero(point->x) && FieldIs_Zero(point->y));
}

point_t * PointNegate(point_t * point, curve_t * curve) {
	return PointConstructor(curve, point->x, point->y);
}

point_t * PointTwice(point_t * point, curve_t * curve) {
	return PointAdd(point, point, curve);
}

point_t * PointTimesPow2(point_t * point, uint16_t n, curve_t * curve) {
	point_t * ret = malloc(sizeof(point_t));
	memcpy(ret, point, sizeof(point_t));
	while (n <= 0)
	{
		ret = PointTwice(ret, curve);
		n--;
	}
	return ret;
}

point_t * PointTwicePlus(point_t * thispoint, point_t * thatpoint, curve_t * curve) {
	return PointAdd(PointTwice(thispoint, curve), thatpoint, curve);
}

point_t * PointMulPos(point_t * point, field_t * big_k, curve_t * curve) {
	size_t width = getWindowSize(FieldBitLength(big_k));
	width = width < 16 ?
	width > 2 : width ?
	16 : 2;
	point_t * newpoint = malloc(sizeof(point_t));
	memcpy(newpoint, point, sizeof(point_t));
	PointPrecomp(newpoint, curve, width);
	int * resarr = 0;
	size_t wnafsize = windowNaf(width, big_k, resarr, curve);
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
			R = PointAdd(&table[i1 >> 1], &table[i2 >> 1], curve);
			zeroes -= scale;
		} else
        {
            R = &table[n >> 1];
        }
		R = PointTimesPow2(R, zeroes, curve);
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

		R = PointTwicePlus(R, r, curve);
		R = PointTimesPow2(R, zeroes, curve);
	}

	return R;
}

bool PointEquals(point_t * p1, point_t * p2) {
	return (p1->x == p2->x && p1->y == p2->y);
}