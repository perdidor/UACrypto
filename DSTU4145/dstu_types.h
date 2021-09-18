/*
 * dstu_types.h
 *
 * Created: 08.09.2021 17:24:15
 *  Author: root
 */ 


#ifndef DSTU_TYPES_H_
#define DSTU_TYPES_H_

#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

typedef enum {
	DSTU_PB_163,
	DSTU_PB_167,
	DSTU_PB_173,
	DSTU_PB_179,
	DSTU_PB_191,
	DSTU_PB_233,
	DSTU_PB_257,
	DSTU_PB_307,
	DSTU_PB_367,
	DSTU_PB_431
} curve_id;

typedef struct {
	bool _is_field;
	int length;
	uint32_t * bytes;
} field_t;

typedef struct {
	field_t x;
	field_t y;
} point_precomputed_t;

typedef struct {
	field_t x;
	field_t y;
	point_precomputed_t twice_point;
	point_precomputed_t precomp_neg[8];
	point_precomputed_t precomp_pos[8];
} point_t;

//typedef struct {
	//size_t Priv_mod_words;
	//uint32_t * mod_bits;
	//uint32_t * mod_tmp;
	//uint32_t * ks;
	//uint32_t * inv_tmp1;
	//uint32_t * inv_tmp2;
	//size_t inv_tmp1_len;
	//size_t inv_tmp2_len;
	//size_t mod_bits_len;
	//size_t ks_len;
	//uint32_t Curve_m;
	//field_t * a;
	//field_t * b;
	//field_t * kofactor;
	//field_t * modulus;
	//field_t * one;
	//field_t * Curve_field_order;
	//field_t * param_a;
	//field_t * param_b;
	//field_t * zero;
	//point_t * Base;
	//field_t * d;
	//uint8_t * sbox;
	//size_t sbox_len;
//} curve_t;

uint8_t DSTUDefaultSBox[128];

#endif /* DSTU_TYPES_H_ */