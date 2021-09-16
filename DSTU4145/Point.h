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
point_t * PointAdd(point_t * thispoint, point_t * p1);
bool PointIsZero(point_t * point);
point_t * PointNegate(point_t * point);
point_t * PointTwice(point_t * point);
point_t * PointTimesPow2(point_t * point, uint16_t n);
point_t * PointTwicePlus(point_t * thispoint, point_t * thatpoint);
point_t * PointMulPos(point_t * point, field_t * big_k);
bool PointEquals(point_t * p1, point_t * p2);

#endif /* POINT_H_ */