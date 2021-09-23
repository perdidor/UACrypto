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

void PointConstructor(field_t * input_x, field_t * input_y, point_t * res);
void PointAdd(point_t * thispoint, point_t * p1, point_t * res);
bool PointIsZero(point_t * point);
void PointNegate(point_t * point);
void PointTwice(point_t * point, point_t * res);
void PointTimesPow2(point_t * point, uint16_t n, point_t * res);
void PointTwicePlus(point_t * thispoint, point_t * thatpoint, point_t * res);
void PointMulPos_Stage1(point_t * point, field_t * big_k, point_t * res);
void PointMulPos_Stage2(point_t * basepoint, field_t * big_k, point_t * stage1res, point_t * res);
void PointMulPos_Stage2_Loop(int32_t wi, point_t * basepoint, point_t * res);
bool PointEquals(point_t * p1, point_t * p2);
void PointCopy(point_t * from, point_t * to);
void FreePoint(point_t * point);

#endif /* POINT_H_ */