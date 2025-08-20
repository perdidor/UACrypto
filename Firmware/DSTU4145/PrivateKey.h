/*
 * PrivateKey.h
 *
 * Created: 09.09.2021 08:58:53
 *  Author: root
 */ 


#ifndef PRIVATEKEY_H_
#define PRIVATEKEY_H_

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include "dstu_types.h"

uint32_t Priv_param_d[8];
uint32_t Priv_mod_bits[3];
uint8_t Priv_mod_words;
uint8_t Priv_sbox[64];

uint32_t Curve_mod_tmp[22];
size_t Curve_m;
size_t Curve_length;
uint32_t Curve_inv_tmp1[9];
uint32_t Curve_inv_tmp2[9];
uint32_t Curve_field_a[10];
uint32_t Curve_field_b[18];
uint32_t Curve_field_kofactor[1];
uint32_t Curve_field_modulus[9];
uint32_t Curve_field_one[9];
uint32_t Curve_field_order[17];
uint32_t Curve_field_zero[1];
uint32_t Curve_basepoint_field_x[18];
uint32_t Curve_basepoint_field_y[9];
uint32_t erand_bytes[9];

precomp_set_t PrivKeyPreComputedPoints;

uint8_t signatureOut[64];

//uint32_t erand_bytes[9];
//
//uint8_t uint8_buffer[128];
//
//uint8_t withzero[33];
//
//field_t curve_order;
//field_t rand_e;
point_t basepoint;

void SignHash(uint8_t * hashvalue);

void Signer_Setup(void);

#endif /* PRIVATEKEY_H_ */