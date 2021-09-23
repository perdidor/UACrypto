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
	FieldFromUint32Buf(&p_x.bytes[0], p_x.length, &res->x);
	FieldFromUint32Buf(&p_y.bytes[0], p_y.length, &res->y);
}

void PointCopy(point_t * from, point_t * to) {
	FieldFromUint32Buf(from->x.bytes, from->x.length, &to->x);
	FieldFromUint32Buf(from->y.bytes, from->y.length, &to->y);
	//memcpy(&to->precomp_neg[0], &from->precomp_neg[0], 8 * sizeof(point_precomputed_t));
	//memcpy(&to->precomp_pos[0], &from->precomp_pos[0], 8 * sizeof(point_precomputed_t));
}

void PointAdd(point_t * thispoint, point_t * p1, point_t * res) {
	field_t param_a, tmp2, invertedtmp, lbdtmp, y0, y1, tmp, invertedx1, tmpy1, y2, tmpx, lbd, x2, x0, x1, tmplbd;

	//if ((res->x.length > 0) && (&res->x != 0)) FreeField(&res->x);
	//if ((res->y.length > 0) && (&res->y != 0)) FreeField(&res->y);

	if (PointIsZero(thispoint)) {
		PointCopy(p1, res);
		//memcpy(res, p1, sizeof(point_t));
		return;
	}
	if (PointIsZero(p1)) {
		//memcpy(res, thispoint, sizeof(point_t));
		PointCopy(thispoint, res);
		return;
	}

	FieldFromUint32Buf(thispoint->y.bytes, thispoint->y.length, &y0);
	FieldFromUint32Buf(p1->y.bytes, p1->y.length, &y1);

	uint32_t x0bytes[10];
	uint32_t x1bytes[10];

	for (int i = 0; i < thispoint->x.length; i++)
	{
		x0bytes[i] = thispoint->x.bytes[i];
	}
	for (int i = 0; i < p1->x.length; i++)
	{
		x1bytes[i] = p1->x.bytes[i];
	}
	uint32_t lbdbytes[10];
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
		//FreeField(&lbdtmp);
		//FreeField(&tmplbd);
		//FreeField(&invertedtmp);
		//FreeField(&tmp2);
		//FreeField(&tmp);
	} else {
		if (!FieldEquals(&y0, &y1) || FieldIs_Zero(&p1->x)) {
			res->x.length = 1;
			res->y.length = 1;
			res->x.bytes = malloc(sizeof(uint32_t));
			res->y.bytes = malloc(sizeof(uint32_t));
			res->x.bytes[0] = 0;
			res->y.bytes[0] = 0;
			//FreeField(&y0);
			//FreeField(&y1);
			//FreeField(&param_a);
			//FreeField(&x1);
			return;
		};
		FieldInvert(&p1->x, &invertedx1);
		FieldMod_Mul(&y1, &invertedx1, &tmpy1);
		FieldAdd(&p1->x, &tmpy1, &lbd);
		for (int i = 0; i < lbd.length; i++)
		{
			lbdbytes[i] = lbd.bytes[i];
		}
		FieldMod_Mul(&lbd, &lbd, &lbdtmp);
		FieldAdd(&lbdtmp, &param_a, &x2);
		FreeField(&lbd);
		FieldFromUint32Buf(lbdbytes, 9, &lbd);
		FieldAddM(&x2, &lbd, 0, 0);
		//FreeField(&invertedx1);
		//FreeField(&lbdtmp);
		//FreeField(&tmpy1);
	}
	FreeField(&lbd);
	FieldFromUint32Buf(lbdbytes, 9, &lbd);
	FieldAdd(&p1->x, &x2, &tmpx);
	FieldMod_Mul(&lbd, &tmpx, &y2);
	FieldAddM(&y2, &x2, 0, 0);
	FieldAddM(&y2, &y1, 0, 0);
	FieldFromUint32Buf(&x2.bytes[0], x2.length, &res->x);
	FieldFromUint32Buf(&y2.bytes[0], y2.length, &res->y);
	res->x.length = x2.length;
	res->y.length = y2.length;
	FreeField(&tmpx);
	FreeField(&param_a);
	FreeField(&x1);
	FreeField(&lbd);
	FreeField(&y0);
	FreeField(&y1);
	FreeField(&x2);
	FreeField(&y2);
	//FreeField(&xf);
	//FreeField(&yf);
	//FreeField(&tmp);
	//FreeField(&y2);
	//FreeField(&tmpx);
	//FreeField(&invertedx1);
	//FreeField(&tmpy1);
	//FreePoint(p2);
	//free(p2);
}

bool PointIsZero(point_t * point) {
	return (FieldIs_Zero(&point->x) && FieldIs_Zero(&point->y));
}

void PointNegate(point_t * point) {
	//return PointConstructor(&point->x, &point->y);
}

void PointTwice(point_t * point, point_t * res) {
	point_t tmp;
	point_t tmp2;
	PointCopy(point, &tmp);
	PointCopy(&tmp, &tmp2);
	PointAdd(&tmp, &tmp2, res);
	FreePoint(&tmp2);
	FreePoint(&tmp);
}

void PointTimesPow2(point_t * point, uint16_t n, point_t * res) {
	point_t tmp;
	point_t tmp2;
	PointCopy(point, &tmp);
	while (n > 0)
	{
		PointTwice(&tmp, &tmp2);
		FreePoint(&tmp);
		PointCopy(&tmp2, &tmp);
		FreePoint(&tmp2);
		n--;
	}
	PointCopy(&tmp, res);
	FreePoint(&tmp);
}

void FreePoint(point_t * point) {
	FreeField(&point->x);
	FreeField(&point->y);
}

void PointTwicePlus(point_t * thispoint, point_t * thatpoint, point_t * res) {
	point_t thistmp;
	point_t thattmp;
	point_t tmp;
	point_t tmpres;
	PointCopy(thispoint, &thistmp);
	PointCopy(thatpoint, &thattmp);
	PointTwice(&thistmp, &tmp);
	PointAdd(&tmp, &thattmp, &tmpres);
	PointCopy(&tmpres, res);
	FreePoint(&thistmp);
	FreePoint(&thattmp);
	FreePoint(&tmp);
	FreePoint(&tmpres);
}

static uint32_t PointMulPosTimes = 0;

void PointMulPos_Stage2_Loop(int32_t wi, point_t * basepoint, point_t * res) {
	point_t r;
	point_t tmp2;
	precomp_t * table = malloc(sizeof(precomp_t) * 8);
	int32_t digit = wi >> 16;
	int zeroes = wi & 0xFFFF;

	int32_t n = abs(digit);
	if (digit < 0) {
		table = &PreComputedPoints.neg[0];
	} else {
		table = &PreComputedPoints.pos[0];
	}

	FieldFromUint32Buf(&table[n >> 1].x[0], 9, &r.x);
	FieldFromUint32Buf(&table[n >> 1].y[0], 9, &r.y);

	point_t tmp;
	PointTwicePlus(res, &r, &tmp);
	PointTimesPow2(&tmp, zeroes, &tmp2);
	PrintDebugUInt32Array(&tmp2.x.bytes[0], tmp2.x.length);
	PrintDebugUInt32Array(&tmp2.y.bytes[0], tmp2.y.length);
	FreePoint(res);
	PointCopy(&tmp2, res);
	free(table);
	FreePoint(&r);
	FreePoint(&tmp2);
	FreePoint(&tmp);
	//free(tmp2);
}

static int32_t resarr[50];

void PointMulPos_Stage2(point_t * basepoint, field_t * big_k, point_t * stage1res, point_t * res) {

	
	while (PointMulPosTimes > 0)
	{
		PointMulPos_Stage2_Loop(resarr[--PointMulPosTimes], basepoint, stage1res);
		if (PointMulPosTimes == 42) break;
	}
	PointCopy(stage1res, res);

}

void PointMulPos_Stage1(point_t * point, field_t * big_k, point_t * res) {
	uint32_t bigkbl = FieldBitLength(big_k);
	uint32_t width = getWindowSize(bigkbl);
	width = width < 16 ?
	(width > 2 ? width : 16) : 2;

	PointMulPosTimes = windowNaf(width, big_k, resarr);

	point_t R;

	int32_t wi = resarr[--PointMulPosTimes];
	int digit = wi >> 16;
	int zeroes = wi & 0xFFFF;

	precomp_t * table = malloc(sizeof(precomp_t) * 8);
	if (digit < 0) {
		table = &PreComputedPoints.neg[0];
		} else {
		table = &PreComputedPoints.pos[0];
	}

	if (PointMulPosTimes > 1) {
		int n = abs(digit);
		if ((n << 2) < (1 << width)) {
			size_t highest = bitLengths[n];
			size_t scale = width - highest;
			size_t lowBits =  n ^ (1 << (highest - 1));

			size_t i1 = ((1 << (width - 1)) - 1);
			size_t i2 = (lowBits << scale) + 1;
			point_t tablei1;
			point_t tablei2;
			FieldFromUint32Buf(&table[i1 >> 1].x[0], 9, &tablei1.x);
			FieldFromUint32Buf(&table[i1 >> 1].y[0], 9, &tablei1.y);
			FieldFromUint32Buf(&table[i2 >> 1].x[0], 9, &tablei2.x);
			FieldFromUint32Buf(&table[i2 >> 1].y[0], 9, &tablei2.y);
			PointAdd(&tablei1, &tablei2, &R);
			zeroes -= scale;
			FreePoint(&tablei1);
			FreePoint(&tablei2);
		} else
        {
			FieldFromUint32Buf(&table[n >> 1].x[0], 9, &R.x);
			FieldFromUint32Buf(&table[n >> 1].x[0], 9, &R.y);
        }

		PointTimesPow2(&R, zeroes, res);
		FreePoint(&R);
		free(table);
	}

}

bool PointEquals(point_t * p1, point_t * p2) {
	return (FieldEquals(&p1->x, &p2->x) && FieldEquals(&p1->y, &p2->y));
}