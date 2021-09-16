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


field_t * FieldFromCurve();
field_t * FieldFromHexStr(char * in_value);
field_t * FieldFromByteArray(uint8_t * in_value, size_t len, size_t max_size);
field_t * FieldFromUint32Buf(uint32_t * in_value, size_t len);

void FieldSetCommonValues(field_t * field);

field_t * FieldMod_Mul(field_t * thisfield, field_t * thatfield);
field_t * FieldMod_Sqr(field_t * thisfield);
void FieldAddM(field_t * thisfield, field_t * thatfield, uint32_t * _from, size_t fromlen);
field_t * FieldAdd(field_t * thatfield);
bool FieldIs_Zero(field_t * field);
bool FieldEquals(field_t * thisfield, field_t * thatfield);
bool FieldIs_Less(field_t * thisfield, field_t * thatfield);
size_t FieldBitLength(field_t * field);
field_t * FieldClearBit(field_t * field, uint32_t n);
field_t * FieldSetBit(field_t * field, uint32_t n);
field_t * FieldShiftRight(field_t * field, uint32_t n);
void FieldShiftRightM(field_t * field, uint32_t n);
uint8_t * FieldBuf8(field_t * field);
uint8_t * FieldTruncate_Buf8(field_t * field);
field_t * FieldInvert(field_t * field);
bool FieldTestBit(uint32_t n, field_t * field);
field_t * FieldClone(field_t * thatfield);
uint8_t FieldTrace(field_t * field);
field_t * FieldTruncate(field_t * field);
field_t * FieldCreateRandom();
field_t * FieldCompress(point_t * point);

#endif /* FIELD_H_ */