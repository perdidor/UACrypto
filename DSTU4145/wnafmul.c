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

void PointPrecomp(point_t * point, curve_t * curve, size_t width) {
	size_t len_off = width - 2 >= 0 ? width - 2 : 0;
	size_t len = 1 << len_off;
	point_t * rpos = (point_t *)(point->_precomp->rpos_points);
	point_t * newrneg_points = (point_t *)(point->_precomp->rneg_points);
	size_t i = point->_precomp->rpos_length;
	if (point->_precomp->rneg_length == 0) {
		newrneg_points = malloc(point->_precomp->rpos_length * sizeof(point_t));
		memcpy(&newrneg_points[0], PointNegate(point, curve), sizeof(point_t));
	}

	if(len == 1) {
		return;
	}

	point_t * twice = (point_t *)point->twice_point;
	if (twice == 0) {
		point->twice_point = (uint8_t *)PointTwice(point, curve);
	}

	for (;i < len; i++) {
		memcpy(&rpos[i], PointAdd(twice, &rpos[i-1], curve), sizeof(point_t));
		memcpy(&newrneg_points[i], PointNegate(&rpos[i], curve), sizeof(point_t));
	}
}