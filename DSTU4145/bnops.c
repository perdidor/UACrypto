/*
 * bnops.c
 *
 * Created: 16.09.2021 17:08:40
 *  Author: root
 */ 
#include <stdbool.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "bnops.h"
#include "dstu_types.h"

void BNFromField(field_t * field, bignumber_t * res) {
	res->words = malloc(field->length * sizeof(uint32_t));
	res->Negative = false;
	res->Length = field->length;
	memcpy(&res->words[0], &field->bytes[0], field->length * sizeof(uint32_t));
}

void BNFromUInt32Buf(uint32_t * array, int datalen, bignumber_t * res) {
	res->words = malloc(datalen * sizeof(uint32_t));
	res->Negative = false;
	res->Length = datalen;
	memcpy(&res->words[0], &array[0], datalen * sizeof(uint32_t));
}

void BNComb10MulTo(bignumber_t * firstnum, bignumber_t * secondnum, bignumber_t * res) {
	uint32_t c = 0;
	uint64_t lo;
	uint64_t mid;
	uint64_t hi;
	uint32_t a0 = firstnum->words[0];
	uint32_t al0 = a0 & 0x1fff;
	uint32_t ah0 = a0 >> 13;
	uint32_t a1 = firstnum->words[1];
	uint32_t al1 = a1 & 0x1fff;
	uint32_t ah1 = a1 >> 13;
	uint32_t a2 = firstnum->words[2];
	uint32_t al2 = a2 & 0x1fff;
	uint32_t ah2 = a2 >> 13;
	uint32_t a3 = firstnum->words[3];
	uint32_t al3 = a3 & 0x1fff;
	uint32_t ah3 = a3 >> 13;
	uint32_t a4 = firstnum->words[4];
	uint32_t al4 = a4 & 0x1fff;
	uint32_t ah4 = a4 >> 13;
	uint32_t a5 = firstnum->words[5];
	uint32_t al5 = a5 & 0x1fff;
	uint32_t ah5 = a5 >> 13;
	uint32_t a6 = firstnum->words[6];
	uint32_t al6 = a6 & 0x1fff;
	uint32_t ah6 = a6 >> 13;
	uint32_t a7 = firstnum->words[7];
	uint32_t al7 = a7 & 0x1fff;
	uint32_t ah7 = a7 >> 13;
	uint32_t a8 = firstnum->words[8];
	uint32_t al8 = a8 & 0x1fff;
	uint32_t ah8 = a8 >> 13;
	uint32_t a9 = firstnum->words[9];
	uint32_t al9 = a9 & 0x1fff;
	uint32_t ah9 = a9 >> 13;
	uint32_t b0 = secondnum->words[0];
	uint32_t bl0 = b0 & 0x1fff;
	uint32_t bh0 = b0 >> 13;
	uint32_t b1 = secondnum->words[1];
	uint32_t bl1 = b1 & 0x1fff;
	uint32_t bh1 = b1 >> 13;
	uint32_t b2 = secondnum->words[2];
	uint32_t bl2 = b2 & 0x1fff;
	uint32_t bh2 = b2 >> 13;
	uint32_t b3 = secondnum->words[3];
	uint32_t bl3 = b3 & 0x1fff;
	uint32_t bh3 = b3 >> 13;
	uint32_t b4 = secondnum->words[4];
	uint32_t bl4 = b4 & 0x1fff;
	uint32_t bh4 = b4 >> 13;
	uint32_t b5 = secondnum->words[5];
	uint32_t bl5 = b5 & 0x1fff;
	uint32_t bh5 = b5 >> 13;
	uint32_t b6 = secondnum->words[6];
	uint32_t bl6 = b6 & 0x1fff;
	uint32_t bh6 = b6 >> 13;
	uint32_t b7 = secondnum->words[7];
	uint32_t bl7 = b7 & 0x1fff;
	uint32_t bh7 = b7 >> 13;
	uint32_t b8 = secondnum->words[8];
	uint32_t bl8 = b8 & 0x1fff;
	uint32_t bh8 = b8 >> 13;
	uint32_t b9 = secondnum->words[9];
	uint32_t bl9 = b9 & 0x1fff;
	uint32_t bh9 = b9 >> 13;

	/* k = 0 */
	lo = al0 * bl0;
	mid = al0 * bh0;
	mid = (mid + ah0 * bl0);
	hi = ah0 * bh0;
	uint32_t w0 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w0 >> 26));
	w0 &= 0x3ffffff;
	/* k = 1 */
	lo = al1 * bl0;
	mid = al1 * bh0;
	mid = (mid + ah1 * bl0);
	hi = ah1 * bh0;
	lo = (lo + al0 * bl1);
	mid = (mid + al0 * bh1);
	mid = (mid + ah0 * bl1);
	hi = (hi + ah0 * bh1);
	uint32_t w1 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w1 >> 26));
	w1 &= 0x3ffffff;
	/* k = 2 */
	lo = al2 * bl0;
	mid = al2 * bh0;
	mid = (mid + ah2 * bl0);
	hi = ah2 * bh0;
	lo = (lo + al1 * bl1);
	mid = (mid + al1 * bh1);
	mid = (mid + ah1 * bl1);
	hi = (hi + ah1 * bh1);
	lo = (lo + al0 * bl2);
	mid = (mid + al0 * bh2);
	mid = (mid + ah0 * bl2);
	hi = (hi + ah0 * bh2);
	uint32_t w2 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w2 >> 26));
	w2 &= 0x3ffffff;
	/* k = 3 */
	lo = al3 * bl0;
	mid = al3 * bh0;
	mid = (mid + ah3 * bl0);
	hi = ah3 * bh0;
	lo = (lo + al2 * bl1);
	mid = (mid + al2 * bh1);
	mid = (mid + ah2 * bl1);
	hi = (hi + ah2 * bh1);
	lo = (lo + al1 * bl2);
	mid = (mid + al1 * bh2);
	mid = (mid + ah1 * bl2);
	hi = (hi + ah1 * bh2);
	lo = (lo + al0 * bl3);
	mid = (mid + al0 * bh3);
	mid = (mid + ah0 * bl3);
	hi = (hi + ah0 * bh3);
	uint32_t w3 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w3 >> 26));
	w3 &= 0x3ffffff;
	/* k = 4 */
	lo = al4 * bl0;
	mid = al4 * bh0;
	mid = (mid + ah4 * bl0);
	hi = ah4 * bh0;
	lo = (lo + al3 * bl1);
	mid = (mid + al3 * bh1);
	mid = (mid + ah3 * bl1);
	hi = (hi + ah3 * bh1);
	lo = (lo + al2 * bl2);
	mid = (mid + al2 * bh2);
	mid = (mid + ah2 * bl2);
	hi = (hi + ah2 * bh2);
	lo = (lo + al1 * bl3);
	mid = (mid + al1 * bh3);
	mid = (mid + ah1 * bl3);
	hi = (hi + ah1 * bh3);
	lo = (lo + al0 * bl4);
	mid = (mid + al0 * bh4);
	mid = (mid + ah0 * bl4);
	hi = (hi + ah0 * bh4);
	uint32_t w4 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w4 >> 26));
	w4 &= 0x3ffffff;
	/* k = 5 */
	lo = al5 * bl0;
	mid = al5 * bh0;
	mid = (mid + ah5 * bl0);
	hi = ah5 * bh0;
	lo = (lo + al4 * bl1);
	mid = (mid + al4 * bh1);
	mid = (mid + ah4 * bl1);
	hi = (hi + ah4 * bh1);
	lo = (lo + al3 * bl2);
	mid = (mid + al3 * bh2);
	mid = (mid + ah3 * bl2);
	hi = (hi + ah3 * bh2);
	lo = (lo + al2 * bl3);
	mid = (mid + al2 * bh3);
	mid = (mid + ah2 * bl3);
	hi = (hi + ah2 * bh3);
	lo = (lo + al1 * bl4);
	mid = (mid + al1 * bh4);
	mid = (mid + ah1 * bl4);
	hi = (hi + ah1 * bh4);
	lo = (lo + al0 * bl5);
	mid = (mid + al0 * bh5);
	mid = (mid + ah0 * bl5);
	hi = (hi + ah0 * bh5);
	uint32_t w5 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w5 >> 26));
	w5 &= 0x3ffffff;
	/* k = 6 */
	lo = al6 * bl0;
	mid = al6 * bh0;
	mid = (mid + ah6 * bl0);
	hi = ah6 * bh0;
	lo = (lo + al5 * bl1);
	mid = (mid + al5 * bh1);
	mid = (mid + ah5 * bl1);
	hi = (hi + ah5 * bh1);
	lo = (lo + al4 * bl2);
	mid = (mid + al4 * bh2);
	mid = (mid + ah4 * bl2);
	hi = (hi + ah4 * bh2);
	lo = (lo + al3 * bl3);
	mid = (mid + al3 * bh3);
	mid = (mid + ah3 * bl3);
	hi = (hi + ah3 * bh3);
	lo = (lo + al2 * bl4);
	mid = (mid + al2 * bh4);
	mid = (mid + ah2 * bl4);
	hi = (hi + ah2 * bh4);
	lo = (lo + al1 * bl5);
	mid = (mid + al1 * bh5);
	mid = (mid + ah1 * bl5);
	hi = (hi + ah1 * bh5);
	lo = (lo + al0 * bl6);
	mid = (mid + al0 * bh6);
	mid = (mid + ah0 * bl6);
	hi = (hi + ah0 * bh6);
	uint32_t w6 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w6 >> 26));
	w6 &= 0x3ffffff;
	/* k = 7 */
	lo = al7 * bl0;
	mid = al7 * bh0;
	mid = (mid + ah7 * bl0);
	hi = ah7 * bh0;
	lo = (lo + al6 * bl1);
	mid = (mid + al6 * bh1);
	mid = (mid + ah6 * bl1);
	hi = (hi + ah6 * bh1);
	lo = (lo + al5 * bl2);
	mid = (mid + al5 * bh2);
	mid = (mid + ah5 * bl2);
	hi = (hi + ah5 * bh2);
	lo = (lo + al4 * bl3);
	mid = (mid + al4 * bh3);
	mid = (mid + ah4 * bl3);
	hi = (hi + ah4 * bh3);
	lo = (lo + al3 * bl4);
	mid = (mid + al3 * bh4);
	mid = (mid + ah3 * bl4);
	hi = (hi + ah3 * bh4);
	lo = (lo + al2 * bl5);
	mid = (mid + al2 * bh5);
	mid = (mid + ah2 * bl5);
	hi = (hi + ah2 * bh5);
	lo = (lo + al1 * bl6);
	mid = (mid + al1 * bh6);
	mid = (mid + ah1 * bl6);
	hi = (hi + ah1 * bh6);
	lo = (lo + al0 * bl7);
	mid = (mid + al0 * bh7);
	mid = (mid + ah0 * bl7);
	hi = (hi + ah0 * bh7);
	uint32_t w7 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w7 >> 26));
	w7 &= 0x3ffffff;
	/* k = 8 */
	lo = al8 * bl0;
	mid = al8 * bh0;
	mid = (mid + ah8 * bl0);
	hi = ah8 * bh0;
	lo = (lo + al7 * bl1);
	mid = (mid + al7 * bh1);
	mid = (mid + ah7 * bl1);
	hi = (hi + ah7 * bh1);
	lo = (lo + al6 * bl2);
	mid = (mid + al6 * bh2);
	mid = (mid + ah6 * bl2);
	hi = (hi + ah6 * bh2);
	lo = (lo + al5 * bl3);
	mid = (mid + al5 * bh3);
	mid = (mid + ah5 * bl3);
	hi = (hi + ah5 * bh3);
	lo = (lo + al4 * bl4);
	mid = (mid + al4 * bh4);
	mid = (mid + ah4 * bl4);
	hi = (hi + ah4 * bh4);
	lo = (lo + al3 * bl5);
	mid = (mid + al3 * bh5);
	mid = (mid + ah3 * bl5);
	hi = (hi + ah3 * bh5);
	lo = (lo + al2 * bl6);
	mid = (mid + al2 * bh6);
	mid = (mid + ah2 * bl6);
	hi = (hi + ah2 * bh6);
	lo = (lo + al1 * bl7);
	mid = (mid + al1 * bh7);
	mid = (mid + ah1 * bl7);
	hi = (hi + ah1 * bh7);
	lo = (lo + al0 * bl8);
	mid = (mid + al0 * bh8);
	mid = (mid + ah0 * bl8);
	hi = (hi + ah0 * bh8);
	uint32_t w8 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w8 >> 26));
	w8 &= 0x3ffffff;
	/* k = 9 */
	lo = al9 * bl0;
	mid = al9 * bh0;
	mid = (mid + ah9 * bl0);
	hi = ah9 * bh0;
	lo = (lo + al8 * bl1);
	mid = (mid + al8 * bh1);
	mid = (mid + ah8 * bl1);
	hi = (hi + ah8 * bh1);
	lo = (lo + al7 * bl2);
	mid = (mid + al7 * bh2);
	mid = (mid + ah7 * bl2);
	hi = (hi + ah7 * bh2);
	lo = (lo + al6 * bl3);
	mid = (mid + al6 * bh3);
	mid = (mid + ah6 * bl3);
	hi = (hi + ah6 * bh3);
	lo = (lo + al5 * bl4);
	mid = (mid + al5 * bh4);
	mid = (mid + ah5 * bl4);
	hi = (hi + ah5 * bh4);
	lo = (lo + al4 * bl5);
	mid = (mid + al4 * bh5);
	mid = (mid + ah4 * bl5);
	hi = (hi + ah4 * bh5);
	lo = (lo + al3 * bl6);
	mid = (mid + al3 * bh6);
	mid = (mid + ah3 * bl6);
	hi = (hi + ah3 * bh6);
	lo = (lo + al2 * bl7);
	mid = (mid + al2 * bh7);
	mid = (mid + ah2 * bl7);
	hi = (hi + ah2 * bh7);
	lo = (lo + al1 * bl8);
	mid = (mid + al1 * bh8);
	mid = (mid + ah1 * bl8);
	hi = (hi + ah1 * bh8);
	lo = (lo + al0 * bl9);
	mid = (mid + al0 * bh9);
	mid = (mid + ah0 * bl9);
	hi = (hi + ah0 * bh9);
	uint32_t w9 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w9 >> 26));
	w9 &= 0x3ffffff;
	/* k = 10 */
	lo = al9 * bl1;
	mid = al9 * bh1;
	mid = (mid + ah9 * bl1);
	hi = ah9 * bh1;
	lo = (lo + al8 * bl2);
	mid = (mid + al8 * bh2);
	mid = (mid + ah8 * bl2);
	hi = (hi + ah8 * bh2);
	lo = (lo + al7 * bl3);
	mid = (mid + al7 * bh3);
	mid = (mid + ah7 * bl3);
	hi = (hi + ah7 * bh3);
	lo = (lo + al6 * bl4);
	mid = (mid + al6 * bh4);
	mid = (mid + ah6 * bl4);
	hi = (hi + ah6 * bh4);
	lo = (lo + al5 * bl5);
	mid = (mid + al5 * bh5);
	mid = (mid + ah5 * bl5);
	hi = (hi + ah5 * bh5);
	lo = (lo + al4 * bl6);
	mid = (mid + al4 * bh6);
	mid = (mid + ah4 * bl6);
	hi = (hi + ah4 * bh6);
	lo = (lo + al3 * bl7);
	mid = (mid + al3 * bh7);
	mid = (mid + ah3 * bl7);
	hi = (hi + ah3 * bh7);
	lo = (lo + al2 * bl8);
	mid = (mid + al2 * bh8);
	mid = (mid + ah2 * bl8);
	hi = (hi + ah2 * bh8);
	lo = (lo + al1 * bl9);
	mid = (mid + al1 * bh9);
	mid = (mid + ah1 * bl9);
	hi = (hi + ah1 * bh9);
	uint32_t w10 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w10 >> 26));
	w10 &= 0x3ffffff;
	/* k = 11 */
	lo = al9 * bl2;
	mid = al9 * bh2;
	mid = (mid + ah9 * bl2);
	hi = ah9 * bh2;
	lo = (lo + al8 * bl3);
	mid = (mid + al8 * bh3);
	mid = (mid + ah8 * bl3);
	hi = (hi + ah8 * bh3);
	lo = (lo + al7 * bl4);
	mid = (mid + al7 * bh4);
	mid = (mid + ah7 * bl4);
	hi = (hi + ah7 * bh4);
	lo = (lo + al6 * bl5);
	mid = (mid + al6 * bh5);
	mid = (mid + ah6 * bl5);
	hi = (hi + ah6 * bh5);
	lo = (lo + al5 * bl6);
	mid = (mid + al5 * bh6);
	mid = (mid + ah5 * bl6);
	hi = (hi + ah5 * bh6);
	lo = (lo + al4 * bl7);
	mid = (mid + al4 * bh7);
	mid = (mid + ah4 * bl7);
	hi = (hi + ah4 * bh7);
	lo = (lo + al3 * bl8);
	mid = (mid + al3 * bh8);
	mid = (mid + ah3 * bl8);
	hi = (hi + ah3 * bh8);
	lo = (lo + al2 * bl9);
	mid = (mid + al2 * bh9);
	mid = (mid + ah2 * bl9);
	hi = (hi + ah2 * bh9);
	uint32_t w11 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w11 >> 26));
	w11 &= 0x3ffffff;
	/* k = 12 */
	lo = al9 * bl3;
	mid = al9 * bh3;
	mid = (mid + ah9 * bl3);
	hi = ah9 * bh3;
	lo = (lo + al8 * bl4);
	mid = (mid + al8 * bh4);
	mid = (mid + ah8 * bl4);
	hi = (hi + ah8 * bh4);
	lo = (lo + al7 * bl5);
	mid = (mid + al7 * bh5);
	mid = (mid + ah7 * bl5);
	hi = (hi + ah7 * bh5);
	lo = (lo + al6 * bl6);
	mid = (mid + al6 * bh6);
	mid = (mid + ah6 * bl6);
	hi = (hi + ah6 * bh6);
	lo = (lo + al5 * bl7);
	mid = (mid + al5 * bh7);
	mid = (mid + ah5 * bl7);
	hi = (hi + ah5 * bh7);
	lo = (lo + al4 * bl8);
	mid = (mid + al4 * bh8);
	mid = (mid + ah4 * bl8);
	hi = (hi + ah4 * bh8);
	lo = (lo + al3 * bl9);
	mid = (mid + al3 * bh9);
	mid = (mid + ah3 * bl9);
	hi = (hi + ah3 * bh9);
	uint32_t w12 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w12 >> 26));
	w12 &= 0x3ffffff;
	/* k = 13 */
	lo = al9 * bl4;
	mid = al9 * bh4;
	mid = (mid + ah9 * bl4);
	hi = ah9 * bh4;
	lo = (lo + al8 * bl5);
	mid = (mid + al8 * bh5);
	mid = (mid + ah8 * bl5);
	hi = (hi + ah8 * bh5);
	lo = (lo + al7 * bl6);
	mid = (mid + al7 * bh6);
	mid = (mid + ah7 * bl6);
	hi = (hi + ah7 * bh6);
	lo = (lo + al6 * bl7);
	mid = (mid + al6 * bh7);
	mid = (mid + ah6 * bl7);
	hi = (hi + ah6 * bh7);
	lo = (lo + al5 * bl8);
	mid = (mid + al5 * bh8);
	mid = (mid + ah5 * bl8);
	hi = (hi + ah5 * bh8);
	lo = (lo + al4 * bl9);
	mid = (mid + al4 * bh9);
	mid = (mid + ah4 * bl9);
	hi = (hi + ah4 * bh9);
	uint32_t w13 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w13 >> 26));
	w13 &= 0x3ffffff;
	/* k = 14 */
	lo = al9 * bl5;
	mid = al9 * bh5;
	mid = (mid + ah9 * bl5);
	hi = ah9 * bh5;
	lo = (lo + al8 * bl6);
	mid = (mid + al8 * bh6);
	mid = (mid + ah8 * bl6);
	hi = (hi + ah8 * bh6);
	lo = (lo + al7 * bl7);
	mid = (mid + al7 * bh7);
	mid = (mid + ah7 * bl7);
	hi = (hi + ah7 * bh7);
	lo = (lo + al6 * bl8);
	mid = (mid + al6 * bh8);
	mid = (mid + ah6 * bl8);
	hi = (hi + ah6 * bh8);
	lo = (lo + al5 * bl9);
	mid = (mid + al5 * bh9);
	mid = (mid + ah5 * bl9);
	hi = (hi + ah5 * bh9);
	uint32_t w14 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w14 >> 26));
	w14 &= 0x3ffffff;
	/* k = 15 */
	lo = al9 * bl6;
	mid = al9 * bh6;
	mid = (mid + ah9 * bl6);
	hi = ah9 * bh6;
	lo = (lo + al8 * bl7);
	mid = (mid + al8 * bh7);
	mid = (mid + ah8 * bl7);
	hi = (hi + ah8 * bh7);
	lo = (lo + al7 * bl8);
	mid = (mid + al7 * bh8);
	mid = (mid + ah7 * bl8);
	hi = (hi + ah7 * bh8);
	lo = (lo + al6 * bl9);
	mid = (mid + al6 * bh9);
	mid = (mid + ah6 * bl9);
	hi = (hi + ah6 * bh9);
	uint32_t w15 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w15 >> 26));
	w15 &= 0x3ffffff;
	/* k = 16 */
	lo = al9 * bl7;
	mid = al9 * bh7;
	mid = (mid + ah9 * bl7);
	hi = ah9 * bh7;
	lo = (lo + al8 * bl8);
	mid = (mid + al8 * bh8);
	mid = (mid + ah8 * bl8);
	hi = (hi + ah8 * bh8);
	lo = (lo + al7 * bl9);
	mid = (mid + al7 * bh9);
	mid = (mid + ah7 * bl9);
	hi = (hi + ah7 * bh9);
	uint32_t w16 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w16 >> 26));
	w16 &= 0x3ffffff;
	/* k = 17 */
	lo = al9 * bl8;
	mid = al9 * bh8;
	mid = (mid + ah9 * bl8);
	hi = ah9 * bh8;
	lo = (lo + al8 * bl9);
	mid = (mid + al8 * bh9);
	mid = (mid + ah8 * bl9);
	hi = (hi + ah8 * bh9);
	uint32_t w17 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w17 >> 26));
	w17 &= 0x3ffffff;
	/* k = 18 */
	lo = al9 * bl9;
	mid = al9 * bh9;
	mid = (mid + ah9 * bl9);
	hi = ah9 * bh9;
	uint32_t w18 = (((c + lo)) + ((mid & 0x1fff) << 13));
	c = (((hi + (mid >> 13))) + (w18 >> 26));
	w18 &= 0x3ffffff;
	res->words = malloc(sizeof(uint32_t) * 20);
	res->Length = 19;
	res->Negative = false;
	res->words[0] = w0;
	res->words[1] = w1;
	res->words[2] = w2;
	res->words[3] = w3;
	res->words[4] = w4;
	res->words[5] = w5;
	res->words[6] = w6;
	res->words[7] = w7;
	res->words[8] = w8;
	res->words[9] = w9;
	res->words[10] = w10;
	res->words[11] = w11;
	res->words[12] = w12;
	res->words[13] = w13;
	res->words[14] = w14;
	res->words[15] = w15;
	res->words[16] = w16;
	res->words[17] = w17;
	res->words[18] = w18;
	if (c != 0) {
		res->words[19] = c;
		res->Length = 20;
	}
}

void BNClone(bignumber_t * bn, bignumber_t * res) {
	res->Length = bn->Length;
	res->Negative = bn->Negative;
	res->words = malloc(sizeof(uint32_t) * bn->Length);
	memcpy(&res->words[0], &bn->words[0], sizeof(uint32_t) * bn->Length);
}

uint32_t BNCountBits(uint32_t num) {
	uint32_t t = num;
	uint32_t r = 0;
	if (t >= 0x1000) {
		r += 13;
		t >>= 13;
	}
	if (t >= 0x40) {
		r += 7;
		t >>= 7;
	}
	if (t >= 0x8) {
		r += 4;
		t >>= 4;
	}
	if (t >= 0x02) {
		r += 2;
		t >>= 2;
	}
	return r + t;
}

void BNiushln(bignumber_t * bn, size_t bits) {
	size_t r = bits % 26;
	size_t h = 0;
	size_t s = ((bits - r) / 26) < bn->Length ? ((bits - r) / 26) : bn->Length;
	uint32_t mask = 0x3ffffff ^ ((0x3ffffff >> r) << r);
	h -= s;
	h = (h > 0) ? h : 0;
	
	if (s == 0) {
		// No-op, we should not move anything at all
		} else if (bn->Length > s) {
			bn->Length -= s;
			for (size_t i = 0; i < bn->Length; i++) {
			bn->words[i] = bn->words[i + s];
		}
		} else {
			bn->words[0] = 0;
			bn->Length = 1;
		}

	uint32_t carry = 0;
	for (size_t i = bn->Length - 1; i >= 0 && (carry != 0 || i >= h); i--) {
		uint32_t word = bn->words[i];
		bn->words[i] = (carry << (26 - r)) | (word >> r);
		carry = word & mask;
	}

	if (bn->Length == 0) {
		bn->words[0] = 0;
		bn->Length = 1;
	}
}

void BNiushrn(bignumber_t * bn, size_t bits) {
	size_t r = bits % 26;
	size_t s = (bits - r) / 26;
	uint32_t carryMask = (0x3ffffff >> (26 - r)) << (26 - r);
	size_t i;

	if (r != 0) {
		uint32_t carry = 0;

		for (i = 0; i < bn->Length; i++) {
			uint32_t newCarry = bn->words[i] & carryMask;
			uint32_t c = ((bn->words[i]) - newCarry) << r;
			bn->words[i] = c | carry;
			carry = newCarry >> (26 - r);
		}

		if (carry) {
			bn->words[i] = carry;
		}
	}

	if (s != 0) {
		for (i = bn->Length - 1; i >= 0; i--) {
			bn->words[i + s] = bn->words[i];
		}

		for (i = 0; i < s; i++) {
			bn->words[i] = 0;
		}
	}
}

void BNishlnsubmul(bignumber_t * bn, bignumber_t * thatbn, size_t mul, size_t shift) {
	uint32_t carry = 0;
	uint32_t w = 0;
	for (size_t i = 0; i < thatbn->Length; i++) {
		w = (bn->words[i + shift]) + carry;
		uint32_t right = (thatbn->words[i]) * mul;
		w -= right & 0x3ffffff;
		carry = (w >> 26) - ((right / 0x4000000));
		bn->words[i + shift] = w & 0x3ffffff;
	}
	for (size_t i = 0; i < bn->Length - shift; i++) {
		w = (bn->words[i + shift]) + carry;
		carry = w >> 26;
		bn->words[i + shift] = w & 0x3ffffff;
	}
	if (carry == 0) return;
	carry = 0;
	for (size_t i = 0; i < bn->Length; i++) {
		w = -(bn->words[i]) + carry;
		carry = w >> 26;
		bn->words[i] = w & 0x3ffffff;
	}
	bn->Negative = true;
}

bool BNIsZero(bignumber_t * bn) {
	return (bn->Length == 1 && bn->words[0] == 0);
}

void BNWordDiv(bignumber_t * thisbn, bignumber_t * thatbn, bignumber_t * res) {
	size_t shift = thisbn->Length - thatbn->Length;
	bignumber_t a;
	BNClone(thisbn, &a);
	uint32_t bhi = thatbn->words[thatbn->Length - 1];
	uint32_t bhiBits = BNCountBits(bhi);
	shift = 26 - bhiBits;
	if (shift != 0) {
		BNiushln(thatbn, shift);
		BNiushln(&a, shift);
		bhi = thatbn->words[thatbn->Length - 1];
	}
	size_t m = a.Length - thatbn->Length;
	bignumber_t diff;
	BNClone(&a, &diff);
	BNishlnsubmul(&diff, thatbn, 1, m);
	if (!diff.Negative) {
		BNClone(&diff, &a);
	}

	for (size_t j = m - 1; j >= 0; j--) {
		uint32_t qj = ((thatbn->Length + j < a.Length) ? a.words[thatbn->Length + j] : 0) * 0x4000000 +
		((thatbn->Length + j < a.Length) ? a.words[thatbn->Length + j - 1] : 0);

		// NOTE: (qj / bhi) is (0x3ffffff * 0x4000000 + 0x3ffffff) / 0x2000000 max
		// (0x7ffffff)
		qj = ((bhi == 0) ? 0 : (qj / bhi)) > 0x3ffffff ? 0x3ffffff : ((bhi == 0) ? 0 : (qj / bhi));
		BNishlnsubmul(&a, thatbn, qj, j);
		//a._ishlnsubmul(b, qj, j);
		while (a.Negative) {
			qj--;
			a.Negative = false;
			BNishlnsubmul(&a, thatbn, 1, j);
			if (!BNIsZero(&a)) {
				a.Negative ^= 1;
			}
		}
	}

	BNClone(&a, res);
}

void BNNeg(bignumber_t * number, bignumber_t * res) {
	BNClone(number, res);
	if (!BNIsZero(res)) {
		res->Negative ^= 1;
	}
}

// Compare two numbers and return:
// 1 - if `this` > `num`
// 0 - if `this` == `num`
// -1 - if `this` < `num`
int8_t BNCmp(bignumber_t * thisnum, bignumber_t * thatnum) {
	if (thisnum->Negative && !thatnum->Negative) return -1;
	if (!thisnum->Negative && thatnum->Negative) return 1;

	int8_t res = BNUCmp(thisnum, thatnum);
	if (thisnum->Negative) return res * -1;
	return res;
};

// Compare two numbers and return:
// 1 - if `this` > `num`
// 0 - if `this` == `num`
// -1 - if `this` < `num`
int8_t BNUCmp(bignumber_t * thisnum, bignumber_t * thatnum) {
	if (thisnum->Length > thatnum->Length) return 1;
	if (thisnum->Length < thatnum->Length) return -1;

	int8_t res = 0;
	for (size_t i = thisnum->Length - 1; i >= 0; i--) {
		uint32_t a = thisnum->words[i];
		uint32_t b = thatnum->words[i];

		if (a == b) continue;
		if (a < b) {
			res = -1;
			} else if (a > b) {
			res = 1;
		}
		break;
	}
	return res;
};

void BNiAdd(bignumber_t * thisbn, bignumber_t * thatbn) {
	uint32_t carry = 0;
	uint32_t r = 0;
	bignumber_t a;
	bignumber_t b;
	if (thisbn->Length > thatbn->Length) {
		BNClone(thisbn, &a);
		BNClone(thatbn, &b);
	} else {
		BNClone(thatbn, &a);
		BNClone(thisbn, &b);
	}
	size_t i = 0;
	for (; i < b.Length; i++) {
		r = (a.words[i] | 0) + (b.words[i] | 0) + carry;
		thisbn->words[i] = r & 0x3ffffff;
		carry = r >> 26;
	}
	for (; carry != 0 && i < a.Length; i++) {
		r = (a.words[i] | 0) + carry;
		thisbn->words[i] = r & 0x3ffffff;
		carry = r >> 26;
	}
	thisbn->Length = a.Length;
	if (carry != 0) {
		thisbn->words[thisbn->Length] = carry;
		thisbn->Length++;
		// Copy the rest of the words
		} else if (BNUCmp(&a, thisbn) != 0) {
		for (; i < a.Length; i++) {
			thisbn->words[i] = a.words[i];
		}
	}
}

void BNAdd(bignumber_t * thisnum, bignumber_t * thatnum, bignumber_t * res) {
	if (thisnum->Length > thatnum->Length) {
		BNClone(thisnum, res);
		BNiAdd(res, thatnum);
		return;
	}
	BNClone(thatnum, res);
	BNiAdd(res, thisnum);
}



void BNDivMod(bignumber_t * thisbn, bignumber_t * thatbn, bool positive, divmodres_t * res) {
	//divmodres_t * res = malloc(sizeof(divmodres_t));
	if (BNIsZero(thisbn)) {
		BNClone(thisbn, res->div);
		BNClone(thisbn, res->mod);
		return;
	}

	if (thisbn->Negative && !thatbn->Negative) {
		bignumber_t s;
		BNNeg(thisbn, &s);
		BNDivMod(&s, thatbn, positive,res);
		bignumber_t mod;
		BNNeg(res->mod, &mod);
		if (positive && mod.Negative) {
			BNiAdd(&mod, thatbn);
		}
		BNClone(&mod, res->mod);
		return;
	}

	if (!thisbn->Negative & thatbn->Negative) {
		bignumber_t s;
		BNNeg(thatbn, &s);
		BNDivMod(thisbn, &s, positive, res);
		return;
	}

	if (thisbn->Negative & thatbn->Negative) {
		bignumber_t s, s2;
		BNNeg(thatbn, &s);
		BNNeg(thisbn, &s2);
		BNDivMod(&s2, &s, positive, res);

		bignumber_t mod;
		BNNeg(res->mod, &mod);
		if (positive && mod.Negative) {
			BNiAdd(&mod, thatbn);
		}

		BNClone(&mod, res->mod);
		return;
	}

	if (thatbn->Length > thisbn->Length || BNCmp(thatbn, thatbn) < 0) {
		res->div = malloc(sizeof(bignumber_t));
		res->mod = thatbn;
		return;
	}

	BNWordDiv(thisbn, thatbn, res->mod);
}
