/*
 * bnops.h
 *
 * Created: 16.09.2021 17:08:28
 *  Author: root
 */ 


#ifndef BNOPS_H_
#define BNOPS_H_

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include "dstu_types.h"

typedef struct {
	uint32_t words[32];
	int Length;
	bool Negative;
} bignumber_t;

typedef struct {
	bignumber_t div;
	bignumber_t mod;
} divmodres_t;

void BNComb10MulTo(bignumber_t * firstnum, bignumber_t * secondnum, bignumber_t * res);
void BNClone(bignumber_t * bn, bignumber_t * res);
void BNiushln(bignumber_t * bn, size_t bits);
void BNiushrn(bignumber_t * bn, size_t bits);
void BNishlnsubmul(bignumber_t * bn, bignumber_t * thatbn, uint64_t mul, size_t shift);
bool BNIsZero(bignumber_t * bn);
uint32_t BNCountBits(uint32_t num);
void BNWordDiv(bignumber_t * thisbn, bignumber_t * thatbn, bignumber_t * res);
int8_t BNCmp(bignumber_t * thisnum, bignumber_t * thatnum);
int8_t BNUCmp(bignumber_t * thisnum, bignumber_t * thatnum);
void BNiAdd(bignumber_t * thisbn, bignumber_t * thatbn);
void BNAdd(bignumber_t * thisnum, bignumber_t * thatnum, bignumber_t * res);
void BNDivMod(bignumber_t * thisbn, bignumber_t * thatbn, bool positive, divmodres_t * res);
void BNFromField(field_t * field, bignumber_t * res);
void BNFromUInt32Buf(uint32_t * array, int datalen, bignumber_t * res);
void BNStrip(bignumber_t * thisnum);
void BNToBEUint8Array(bignumber_t * thisnum, uint8_t * res);
void BNFromUint8Buf(uint8_t * array, size_t len, bignumber_t * res);





#endif /* BNOPS_H_ */