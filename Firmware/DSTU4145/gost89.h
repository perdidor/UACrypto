/*
 * gost89.h
 *
 * Created: 12.09.2021 07:30:53
 *  Author: root
 */ 


#ifndef GOST89_H_
#define GOST89_H_

#include <math.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
	uint32_t k[8];

	uint32_t k1[16];
	uint32_t k2[16];
	uint32_t k3[16];
	uint32_t k4[16];
	uint32_t k5[16];
	uint32_t k6[16];
	uint32_t k7[16];
	uint32_t k8[16];

	uint32_t k87[256];
	uint32_t k65[256];
	uint32_t k43[256];
	uint32_t k21[256];

	uint32_t n[2];

	uint8_t gamma[8];
} gost89_t;

//=========GOST89 hash properties
int gost89HashLength;
uint8_t gost89Hash_U[32];
uint8_t gost89Hash_V[32];
uint8_t gost89Hash_W[32];
uint8_t gost89Hash__S[32];
uint8_t gost89Hash_C8Buf[8];
uint8_t gost89Hash_S[32];
uint8_t gost89Hash_Value[32];
uint8_t gost89Hash_Buf[32];
int32_t gost89Hash_AB2[4];
uint8_t gost89Hash_Left[64];
int gost89Hash_Leftlength;
//=========GOST89 hash properties

gost89_t gost89;

uint8_t gost89_Key[32];

void gost89_init(uint8_t * sbox);
void gost89BoxInit(void);
uint32_t gost89Pass(uint32_t x);
void gost89Crypt64(uint8_t * clear, uint8_t * outres);
void gost89Decrypt64(uint8_t * crypt, uint8_t * outres);
void gost89Crypt64_CFB(uint8_t * iv, uint8_t * plain, uint8_t * outres);
void gost89Decrypt64_CFB(uint8_t * iv, uint8_t * crypted, uint8_t * outres);
void gost89Crypt_CFB(uint8_t * iv, uint8_t * plain, int plainlen, uint8_t * resarray);
void gost89Decrypt_CFB(uint8_t * iv, uint8_t * crypted, int cryptedlen, uint8_t * resarray);
void gost89Crypt(uint8_t * iv, uint8_t * plain, int plainlen, uint8_t * resarray);
void gost89Decrypt(uint8_t * crypted, int cryptedlen, uint8_t * resarray);
void gost89DecodeData(uint8_t * crypted, int cryptlen, uint8_t * pw, int pwlen, uint8_t * plain);
int gost89MacOut(uint8_t * buf, int nbits, uint8_t * outres);
void Gost89ConvertPassword(uint8_t * pw, int pwlen);
void gost89Mac64(uint8_t * datablock, uint8_t * outres);
void gost89Mac(uint8_t * data, int datalen, uint8_t * outres);

void Gost89HashSwapBytes(uint8_t * w, uint8_t * k);
void Gost89HashCircle_XOR8(uint8_t * w, uint8_t * k);
void Gost89HashTransform_3(uint8_t *  data);
int32_t Gost89HashAddBlocks(int n, uint8_t * left, uint8_t * right);
void Gost89HashXorBlocks(uint8_t * ret, uint8_t * a, int alen, uint8_t * b);
void Gost89HashStep(uint8_t * H, uint8_t * Curve_m);
void Gost89HashUpdate(uint8_t * block, int blocklen);
void Gost89HashUpdate32(uint8_t * block32);
void Gost89HashFinish(void);
void Gost89HashReset(void);
void Gost89HashCompute(uint8_t * data, int datalen);
void Gost89HashDumb_KDF(uint8_t * input, int inputlen, int n_passes);
void Gost89HashPB_KDF(uint8_t * input, int inputlen, uint8_t * salt, int saltlen, int iterations, uint8_t * reskey);
void Gost89HashZeroAll(void);

#endif /* GOST89_H_ */