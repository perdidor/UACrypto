/*
 * wnafmul.c
 *
 * Created: 08.09.2021 14:34:29
 *  Author: root
 */ 
#include "wnafmul.h"
#include "wnaf.h"
#include "dstu_types.h"
#include "Point.h"
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

uint8_t bitLengths[256] = {
	0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

void PointPrecomp(point_t * point, uint32_t width) {
	uint32_t len_off = (width >= 2) ? (width - 2) : 0;
	uint8_t len = 1 << len_off;
	//point_t * rpos = &point->precomp_pos;
	//point_t * newrneg_points = malloc(sizeof(point_precomputed_t) * 8);
	//memcpy(newrneg_points, &point->precomp_neg, sizeof(point_precomputed_t) * 8);
	size_t i = 9;
	if (PointIsZero((point_t *)&point->precomp_neg[0])) {
		//newrneg_points = malloc(point->_precomp->rpos_length * sizeof(point_t));
		memcpy(&point->precomp_neg[0], PointNegate(point), sizeof(point_t));
	}

	if(len == 1) {
		return;
	}

	if (PointIsZero((point_t *)&point->twice_point)) {
		PointTwice(point, point);
		memcpy(&point->twice_point, point, sizeof(point_t));
	}

	for (;i < len; i++) {
		PointAdd((point_t *)&point->twice_point, (point_t *)&point->precomp_neg[i-1], (point_t *)&point->twice_point);
		memcpy(&point->precomp_neg[i], &point->twice_point, sizeof(point_t));
		memcpy(&point->precomp_neg[i], PointNegate((point_t *)&point->precomp_neg[i]), sizeof(point_t));
	}
}