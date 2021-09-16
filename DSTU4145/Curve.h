/*
 * .h
 *
 * Created: 04.09.2021 17:52:21
 *  Author: root
 */ 
#ifndef CURVE_H_
#define CURVE_H_

#include "Point.h"
#include "dstu_types.h"
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

uint32_t * CurveCalc_Modulus();

void CurveExpandXtoY(field_t * x, field_t * expanded_x, field_t * expanded_y);
field_t * Curvefsquad(field_t * field);
field_t * Curvefsquad_odd(field_t * field);
bool CurveContainsPoint(point_t * point);
void CurveSetBase(field_t * base_x, field_t * base_y);
field_t * CurveCompModulus(uint32_t Curve_m, uint32_t * ks, size_t ks_len);


#endif /* CURVE_H_ */