/*
 * Point.h
 *
 * Created: 08.09.2021 16:22:59
 *  Author: root
 */ 

#ifndef POINT_H_
#define POINT_H_

#include "Field.h"
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

point_t * PointConstructor(field_t * input_x, field_t * input_y);
void PointAdd(point_t * thispoint, point_t * p1, point_t * res);
bool PointIsZero(point_t * point);
point_t * PointNegate(point_t * point);
void PointTwice(point_t * point, point_t * res);
point_t * PointTimesPow2(point_t * point, uint16_t n);
void PointTwicePlus(point_t * thispoint, point_t * thatpoint, point_t * res);
void PointMulPos(point_t * point, field_t * big_k, point_t * res);
bool PointEquals(point_t * p1, point_t * p2);

uint32_t suka(field_t * asd);
void PrintDebugUInt32Array(uint32_t * arr, uint32_t arrlen);

#endif /* POINT_H_ */