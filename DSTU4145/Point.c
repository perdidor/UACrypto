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
#include "USART.h"

void PointConstructor(field_t * input_x, field_t * input_y, point_t * res) {
	field_t p_x;
	field_t p_y;
	FieldClone(&p_x, input_x);
	FieldClone(&p_y, input_y);
	if (input_y == 0) {
		CurveExpandXtoY(input_x, &p_x, &p_y);
	}
	//point_t * res = malloc(sizeof(point_t));
	//memcpy(&res->x, p_x, sizeof(field_t));
	//memcpy(&res->y, p_y, sizeof(field_t));
	FieldFromUint32Buf(&p_x.bytes[0], p_x.length, &res->x);
	FieldFromUint32Buf(&p_y.bytes[0], p_y.length, &res->y);
	//free(p_x);
	//free(p_y);
	//return res;
}

void PointCopy(point_t * from, point_t * to) {
	FieldFromUint32Buf(&from->x.bytes[0], from->x.length, &to->x);
	FieldFromUint32Buf(&from->y.bytes[0], from->y.length, &to->y);
	memcpy(&to->precomp_neg[0], &from->precomp_neg[0], 8 * sizeof(point_precomputed_t));
	memcpy(&to->precomp_pos[0], &from->precomp_pos[0], 8 * sizeof(point_precomputed_t));
}

void PointAdd(point_t * thispoint, point_t * p1, point_t * res) {
	field_t param_a, tmp2, invertedtmp, lbdtmp, y0, y1, xf, yf, tmp;
	if (PointIsZero(thispoint)) {
		memcpy(res, p1, sizeof(point_t));
		return;
	}
	if (PointIsZero(p1)) {
		memcpy(res, thispoint, sizeof(point_t));
		return;
	}
	xf.length = 0;
	yf.length = 0;
	//FieldFromUint32Buf(Curve_field_zero, 1, &xf);
	//FieldFromUint32Buf(Curve_field_zero, 1, &yf);
	point_t * p2 = malloc(sizeof(point_t));
	FieldFromUint32Buf(Curve_field_zero, 1, &p2->x);
	FieldFromUint32Buf(Curve_field_zero, 1, &p2->y);
	//memset(p2, 0x00, sizeof(point_t));
	//PointConstructor(&xf, &yf, &p2);

	FieldFromUint32Buf(thispoint->y.bytes, thispoint->y.length, &y0);
	FieldFromUint32Buf(p1->y.bytes, p1->y.length, &y1);

	static uint32_t x0bytes[10];
	static uint32_t x1bytes[10];
	static field_t lbd, x2, x0, x1, tmplbd;
	for (int i = 0; i < thispoint->x.length; i++)
	{
		x0bytes[i] = thispoint->x.bytes[i];
	}
	for (int i = 0; i < p1->x.length; i++)
	{
		x1bytes[i] = p1->x.bytes[i];
	}
	static uint32_t lbdbytes[10];
	FieldFromUint32Buf(x1bytes, 10, &x1);
	FieldFromUint32Buf(Curve_field_a, 10, &param_a);
	if (!FieldEquals(&thispoint->x, &p1->x)) {
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
		FieldFromUint32Buf(x0bytes, 10, &x0);
		FieldAddM(&x2, &x0, 0, 0);
		FieldAddM(&x2, &p1->x, 0, 0);
	} else {
		//PrintDebugUInt32Array(&y0.bytes[0], y0.length);
		//PrintDebugUInt32Array(&y1.bytes[0], y1.length);
		//PrintDebugUInt32Array(&p1->x.bytes[0], p1->x.length);
		if (!FieldEquals(&y0, &y1) || FieldIs_Zero(&p1->x)) {
			res->x.length = p2->x.length;
			res->y.length = p2->y.length;
			FieldFromUint32Buf(&p2->x.bytes[0], p2->x.length, &res->x);
			FieldFromUint32Buf(&p2->y.bytes[0], p2->y.length, &res->y);
			return;
		};
		field_t invertedx1, lbdtmp, tmpy1;
		FieldInvert(&p1->x, &invertedx1);
		FieldMod_Mul(&y1, &invertedx1, &tmpy1);
		FieldAdd(&p1->x, &tmpy1, &lbd);
		FieldMod_Mul(&lbd, &lbd, &lbdtmp);
		FieldAdd(&lbdtmp, &param_a, &x2);
		for (int i = 0; i < lbd.length; i++)
		{
			lbdbytes[i] = lbd.bytes[i];
		}
		FieldAddM(&x2, &lbd, 0, 0);
	}
	field_t y2, tmpx;
	FieldFromUint32Buf(lbdbytes, 9, &lbd);
	//PrintDebugUInt32Array(&lbd.bytes[0], lbd.length);
	FieldAdd(&p1->x, &x2, &tmpx);
	FieldMod_Mul(&lbd, &tmpx, &y2);
	FieldAddM(&y2, &x2, 0, 0);
	FieldAddM(&y2, &y1, 0, 0);
	//memcpy(&res->x, &x2, x2.length);
	//memcpy(&res->y, &y2, y2.length);
	FieldFromUint32Buf(&x2.bytes[0], x2.length, &res->x);
	FieldFromUint32Buf(&y2.bytes[0], y2.length, &res->y);
	res->x.length = x2.length;
	res->y.length = y2.length;
}

bool PointIsZero(point_t * point) {
	return (FieldIs_Zero(&point->x) && FieldIs_Zero(&point->y));
}

void PointNegate(point_t * point) {
	//return PointConstructor(&point->x, &point->y);
}

void PointTwice(point_t * point, point_t * res) {
	//point_t * tmp = malloc(sizeof(point_t));
	point_t tmp, tmp2;
	PointCopy(point, &tmp);
	PointCopy(point, &tmp2);
	//FieldFromUint32Buf(&point->x.bytes[0], point->x.length, &tmp->x);
	//FieldFromUint32Buf(&point->y.bytes[0], point->y.length, &tmp->y);
	//PrintDebugUInt32Array(&point->x.bytes[0], point->x.length);
	//PrintDebugUInt32Array(&tmp->x.bytes[0], tmp->x.length);
	PointAdd(&tmp, &tmp2, res);
	//free(tmp);
}

void PointTimesPow2(point_t * point, uint16_t n, point_t * res) {
	point_t tmp, tmp2;
	PointCopy(point, &tmp);
	PointCopy(point, &tmp2);
	//FieldFromUint32Buf(&point->x.bytes[0], point->x.length, &tmp.x);
	//FieldFromUint32Buf(&point->y.bytes[0], point->y.length, &tmp.y);
	while (n > 0)
	{
		PointTwice(&tmp, &tmp2);
		PointCopy(&tmp2, &tmp);
		//FieldFromUint32Buf(&res->x.bytes[0], res->x.length, &tmp.x);
		//FieldFromUint32Buf(&res->y.bytes[0], res->y.length, &tmp.y);
		n--;
	}
	PointCopy(&tmp, res);
}

void PointTwicePlus(point_t * thispoint, point_t * thatpoint, point_t * res) {
	point_t thistmp, thattmp, tmp, tmpres;
	PointCopy(thispoint, &thistmp);
	PointCopy(thatpoint, &thattmp);
	PointCopy(thatpoint, &tmp);
	//FieldFromUint32Buf(&thispoint->x.bytes[0], thispoint->x.length, &thistmp.x);
	//FieldFromUint32Buf(&thispoint->y.bytes[0], thispoint->y.length, &thistmp.y);
	//FieldFromUint32Buf(&thatpoint->x.bytes[0], thispoint->x.length, &thattmp.x);
	//FieldFromUint32Buf(&thatpoint->y.bytes[0], thispoint->y.length, &thattmp.y);
	PointTwice(&thistmp, &tmp);
	PointAdd(&tmp, &thattmp, &tmpres);
	PointCopy(&tmpres, res);
	//free(tmp);
}

void PointMulPos(point_t * point, field_t * big_k, point_t * res) {
	//PrintDebugUInt32Array(&big_k->bytes[0], big_k->length);
	uint32_t bigkbl = FieldBitLength(big_k);
	uint32_t width = getWindowSize(bigkbl);
	width = width < 16 ?
	(width > 2 ? width : 16) : 2;
	//size_t resarrsize = floor(bigkbl / width + 1);
	point_t newpoint;
	int32_t resarr[50];
	PointCopy(point, &newpoint);
	//int32_t * tmp = malloc(sizeof(int32_t) * 60);
	//PrintDebugUInt32Array(&big_k->bytes[0], big_k->length);
	uint32_t wnafsize = windowNaf(width, big_k, resarr);
	uint32_t asd = windowNaf(width, big_k, resarr);
	//wnafsize = tmpwnafsize;
	
	//wnafsize--;
	//memcpy(&resarr[0], &tmp[0], sizeof(int32_t) * tmpwnafsize);
	//wnafsize--;
	//free(tmp);
	point_t R;
	point_t R2;
	int zeroes = 0;
	if (asd > 1) {
		int32_t wi = resarr[--asd];
		int digit = wi >> 16;
		zeroes = wi & 0xFFFF;
		int n = abs(digit);
		point_precomputed_t table[8];
		if (digit < 0) {
			memcpy(&table, newpoint.precomp_neg, sizeof(point_precomputed_t) * 8);
		} else {
			memcpy(&table, newpoint.precomp_pos, sizeof(point_precomputed_t) * 8);
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

		//PrintDebugUInt32Array(&R.x.bytes[0], R.x.length);
		//PrintDebugUInt32Array(&R.y.bytes[0], R.y.length);
		PointTimesPow2(&R, zeroes, &R2);
		//PrintDebugUInt32Array(&R2.x.bytes[0], R2.x.length);
		//PrintDebugUInt32Array(&R2.y.bytes[0], R2.y.length);
		//PointCopy(&tmp, &R);
	}

	//int32_t wi = 0;
	//int digit = 0;
	//int n = 0;
	//point_precomputed_t * table = malloc(sizeof(point_precomputed_t) * 8);
	//point_t r;
	while (asd > 0)
	{
		point_t r;
		point_precomputed_t table[8];
		int32_t wi = resarr[--asd];
		int32_t digit = wi >> 16;
		zeroes = wi & 0xFFFF;

		int32_t n = abs(digit);
		if (digit < 0) {
			memcpy(&table, &newpoint.precomp_neg[0], sizeof(point_precomputed_t) * 8);
			} else {
			memcpy(&table, &newpoint.precomp_pos[0], sizeof(point_precomputed_t) * 8);
		}

		FieldFromUint32Buf(&table[n >> 1].x.bytes[0], 9, &r.x);
		FieldFromUint32Buf(&table[n >> 1].y.bytes[0], 9, &r.y);
		//PrintDebugUInt32Array(&r.x.bytes[0], r.x.length);
		//PrintDebugUInt32Array(&r.y.bytes[0], r.y.length);
		//PrintDebugUInt32Array(&R2.x.bytes[0], R2.x.length);
		//PrintDebugUInt32Array(&R2.y.bytes[0], R2.y.length);
		point_t tmp;
		PointTwicePlus(&R2, &r, &tmp);
		//PrintDebugUInt32Array(&tmp.x.bytes[0], tmp.x.length);
		//PrintDebugUInt32Array(&tmp.y.bytes[0], tmp.y.length);
		point_t tmp2;
		PointTimesPow2(&tmp, zeroes, &tmp2);
		PrintDebugUInt32Array(&tmp2.x.bytes[0], tmp2.x.length);
		PrintDebugUInt32Array(&tmp2.y.bytes[0], tmp2.y.length);
		PointCopy(&tmp2, res);
	}

	//return R;
}

bool PointEquals(point_t * p1, point_t * p2) {
	return (&p1->x == &p2->x && &p1->y == &p2->y);
}