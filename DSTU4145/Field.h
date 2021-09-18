/*
 * Field.h
 *
 * Created: 04.09.2021 17:56:20
 *  Author: root
 */ 
#ifndef FIELD_H_
#define FIELD_H_

#include "Curve.h"
#include "dstu_types.h"
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>


void FieldFromCurve(field_t * newfield);
void FieldFromHexStr(char * in_value, field_t * newfield);
void FieldFromByteArray(uint8_t * in_value, size_t len, size_t max_size, field_t * res);
void FieldFromUint32Buf(uint32_t * in_value, size_t len, field_t * res);


void FieldMod_Mul(field_t * thisfield, field_t * thatfield, field_t * res);
void FieldMod_Sqr(field_t * thisfield, field_t * res);
void FieldAddM(field_t * thisfield, field_t * thatfield, uint32_t * _from, int fromlen);
void FieldAdd(field_t * thisfield, field_t * thatfield, field_t * res);
bool FieldIs_Zero(field_t * field);
bool FieldEquals(field_t * thisfield, field_t * thatfield);
bool FieldIs_Less(field_t * thisfield, field_t * thatfield);
uint32_t FieldBitLength(field_t * field);
void FieldClearBit(field_t * field, uint32_t n);
void FieldSetBit(field_t * field, uint32_t n);
void FieldShiftRight(field_t * field, uint32_t n, field_t * res);
void FieldShiftRightM(field_t * field, uint32_t n);
uint8_t * FieldBuf8(field_t * field);
uint8_t * FieldTruncate_Buf8(field_t * field);
void FieldInvert(field_t * field, field_t * res);
bool FieldTestBit(uint32_t n, field_t * field);
void FieldClone(field_t * thatfield, field_t * res);
uint8_t FieldTrace(field_t * field);
void FieldTruncate(field_t * field, field_t * res);
void FieldCreateRandom(field_t * ret);
field_t * FieldCompress(point_t * point);

#endif /* FIELD_H_ */