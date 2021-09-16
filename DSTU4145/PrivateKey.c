/*
 * PrivateKey.c
 *
 * Created: 09.09.2021 08:59:05
 *  Author: root
 */ 

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "dstu_types.h"
#include "PrivateKey.h"
#include "Field.h"
#include "Point.h"


uint32_t Priv_param_d[8] = { 2082397019, 1468370442, 823178231, 2862269997, 2052372100, 4252500519, 3735157276, 574723566 };
uint32_t Priv_mod_bits[3] = { 257, 12, 0 };
size_t Priv_mod_words = 9;
uint8_t Priv_sbox[64] = { 169, 214, 235, 69, 241, 60, 112, 130, 128, 196, 150, 123, 35, 31, 94, 173, 246, 88, 235, 164, 192, 55, 41, 29, 56, 217, 107, 240, 37, 202, 78, 23, 248, 233, 114, 13, 198, 21, 180, 58, 40, 151, 95, 11, 193, 222, 163, 100, 56, 181, 100, 234, 44, 23, 159, 208, 18, 62, 109, 184, 250, 197, 121, 4 };

uint32_t Curve_mod_tmp[22] = { 480272844, 1120844731, 4021956850, 1996059362, 3211450189, 3878564784, 1855574317, 1081189957, 408236217, 1101498817, 3883717065, 2138901211, 1364682961, 2798888508, 2476576128, 109734631, 0, 0, 0, 0, 0, 0 };
size_t Curve_m = 257;
size_t Curve_length = 8;
uint32_t Curve_inv_tmp1[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_inv_tmp2[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_field_a[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_field_b[18] = { 3689135632, 530508394, 1166825350, 4287899073, 2811503508, 418986199, 18179454, 3472135282, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_field_kofactor[1] = { 4 };
uint32_t Curve_field_modulus[9] = { 4097, 0, 0, 0, 0, 0, 0, 0, 2 };
uint32_t Curve_field_one[9] = { 1, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_field_order[17] = { 2424129293, 3554768660, 4051888519, 1733894458, 0, 0, 0, 2147483648, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t Curve_field_zero[1] = { 0 };
uint32_t Curve_basepoint_field_x[18] = { 3637645239, 883550428, 454084131, 2059702730, 812416512, 1439507979, 2098109292, 707391264, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t Curve_basepoint_field_y[9] = { 1457001711, 2339384561, 2849377175, 3867225235, 4003475714, 2681009880, 4148483140, 109499423, 1 };

void SignHash(uint8_t * hashvalue) {
	field_t * hash_v = FieldFromByteArray(hashvalue, 32, 32);
	field_t * rand_e = FieldCreateRandom();
	field_t * basex = FieldFromUint32Buf(Curve_basepoint_field_x, 18);
	basex->length = 18;
	field_t * basey = FieldFromUint32Buf(Curve_basepoint_field_y, 9);
	basey->length = 9;
	point_t * basepoint = PointConstructor(basex, basey);
	point_t * eG = PointMulPos(basepoint, rand_e);
	field_t * r = FieldMod_Mul(hash_v, eG->x);
	field_t * r2 = FieldTruncate(r);
	free(r);
	
}
