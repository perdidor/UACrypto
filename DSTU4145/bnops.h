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

typedef struct {
	uint32_t * words;
	size_t Length;
	bool Negative;
} bignumber_t;

typedef struct {
	bignumber_t * div;
	bignumber_t * mod;
} divmodres_t;

void BNComb10MulTo(bignumber_t * firstnum, bignumber_t * secondnum, bignumber_t * res);
void BNClone(bignumber_t * bn, bignumber_t * res);
void BNiushln(bignumber_t * bn, size_t bits);
void BNiushln(bignumber_t * bn, size_t bits);
void BNishlnsubmul(bignumber_t * bn, bignumber_t * thatbn, size_t mul, size_t shift);
bool BNIsZero(bignumber_t * bn);
uint32_t BNCountBits(uint32_t num);
void BNWordDiv(bignumber_t * thisbn, bignumber_t * thatbn, bignumber_t * res);
int8_t BNCmp(bignumber_t * thisnum, bignumber_t * thatnum);
int8_t BNUCmp(bignumber_t * thisnum, bignumber_t * thatnum);
void BNiAdd(bignumber_t * thisbn, bignumber_t * thatbn);
void BNAdd(bignumber_t * thisnum, bignumber_t * thatnum, bignumber_t * res);
void BNDivMod(bignumber_t * thisbn, bignumber_t * thatbn, bool positive, divmodres_t * res);





#endif /* BNOPS_H_ */