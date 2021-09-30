/*
 * gf2m.h
 *
 * Created: 04.09.2021 18:22:03
 *  Author: root
 */ 


#ifndef GF2M_H_
#define GF2M_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t g2fmblength(uint32_t * bytes, int len);
void g2fmshiftRight(uint32_t * bytes, size_t len, uint32_t right);
void g2fmmul_1x1(uint32_t * ret, uint16_t offset, uint32_t a, uint32_t b);
void g2fmmul_2x2(uint32_t a1, uint32_t a0, uint32_t b1, uint32_t b0, uint32_t * ret);
void g2fmfmul(uint32_t * a, size_t alen, uint32_t * b, size_t blen, uint32_t * s, size_t slen);
void g2fmffmod(uint32_t * a, size_t alen, uint32_t * p, size_t p_len, uint32_t * ret);
void g2fmfinv(uint32_t * a, size_t alen, uint32_t * p, size_t plen, uint32_t * ret);



#endif /* GF2M_H_ */