/*
 * gost89.c
 *
 * Created: 12.09.2021 07:31:03
 *  Author: root
 */ 
#define F_CPU	8000000UL

#include <util/delay.h>
#include "gost89.h"
#include "DSTU.h"
#include "USART.h"
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

size_t gost89HashLength;
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
size_t gost89Hash_Leftlength;

gost89_t gost89;

uint8_t gost89_Key[32];

void ZeroAll() {
	memset(gost89Hash_U, 0, 32);
	memset(gost89Hash_V, 0, 32);
	memset(gost89Hash_W, 0, 32);
	memset(gost89Hash__S, 0, 32);
	memset(gost89_Key, 0, 32);
	memset(gost89Hash_C8Buf, 0, 8);
	memset(gost89Hash_S, 0, 32);
	memset(gost89Hash_Value, 0, 32);
	memset(gost89Hash_Buf, 0, 32);
	memset(gost89Hash_AB2, 0, 4);
	memset(gost89Hash_Left, 0, 64);
	memset(gost89_Key, 0, 32);
}

void gost89_init(uint8_t * sbox) {
	if (sbox == 0) {
		sbox = DSTUDefaultSBox;
	}
	uint8_t i = 0;
	for (; i < 16; i++)
	{
		gost89.k8[i] = sbox[i];
		gost89.k7[i] = sbox[i + 16];
		gost89.k6[i] = sbox[i + 32];
		gost89.k5[i] = sbox[i + 48];
		gost89.k4[i] = sbox[i + 64];
		gost89.k3[i] = sbox[i + 80];
		gost89.k2[i] = sbox[i + 96];
		gost89.k1[i] = sbox[i + 112];
	}
	gost89BoxInit();
}

void gost89BoxInit() {
	for (int i = 0; i < 256; i++)
	{
		gost89.k87[i] = (gost89.k8[i >> 4] << 4 | gost89.k7[i & 15]) << 24;
		gost89.k65[i] = (gost89.k6[i >> 4] << 4 | gost89.k5[i & 15]) << 16;
		gost89.k43[i] = (gost89.k4[i >> 4] << 4 | gost89.k3[i & 15]) << 8;
		gost89.k21[i] = (gost89.k2[i >> 4] << 4 | gost89.k1[i & 15]);
	}
}

uint32_t gost89Pass(uint32_t x) {
	x = gost89.k87[(x >> 24) & 255] | gost89.k65[(x >> 16) & 255] | gost89.k43[(x >> 8) & 255] | gost89.k21[x & 255];
	return x << 11 | (x >> 21);
}

void gost89Crypt64(uint8_t * clear, uint8_t * outres) {

	gost89.n[0] = ((uint32_t)clear[0]|((uint32_t)clear[1]<<8)|((uint32_t)clear[2]<<16)|((uint32_t)clear[3]<<24));
	gost89.n[1] = ((uint32_t)clear[4]|((uint32_t)clear[5]<<8)|((uint32_t)clear[6]<<16)|((uint32_t)clear[7]<<24));
	/* Instead of swappclearg halves, swap names each round */

	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[0]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[1]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[2]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[3]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[4]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[5]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[6]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[7]);

	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[0]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[1]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[2]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[3]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[4]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[5]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[6]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[7]);

	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[0]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[1]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[2]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[3]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[4]); 
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[5]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[6]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[7]);

	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[7]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[6]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[5]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[4]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[3]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[2]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[1]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[0]);

	outres[0] = (uint8_t)(gost89.n[1] & 0xff);
	outres[1] = (uint8_t)((gost89.n[1] >> 8) & 0xff);
	outres[2] = (uint8_t)((gost89.n[1] >> 16) & 0xff);
	outres[3] = (uint8_t)(gost89.n[1] >> 24);
	outres[4] = (uint8_t)(gost89.n[0] & 0xff);
	outres[5] = (uint8_t)((gost89.n[0] >> 8) & 0xff);
	outres[6] = (uint8_t)((gost89.n[0] >> 16) & 0xff);
	outres[7] = (uint8_t)(gost89.n[0] >> 24);
}

void gost89Decrypt64(uint8_t * crypt, uint8_t * outres) {
	gost89.n[0] = ((uint32_t)crypt[0] | ((uint32_t)crypt[1] << 8) | ((uint32_t)crypt[2] << 16) | ((uint32_t)crypt[3] << 24));
	gost89.n[1] = ((uint32_t)crypt[4] | ((uint32_t)crypt[5] << 8) | ((uint32_t)crypt[6] << 16) | ((uint32_t)crypt[7] << 24));

	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[0]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[1]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[2]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[3]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[4]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[5]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[6]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[7]);

	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[7]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[6]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[5]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[4]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[3]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[2]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[1]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[0]);

	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[7]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[6]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[5]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[4]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[3]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[2]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[1]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[0]);

	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[7]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[6]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[5]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[4]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[3]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[2]);
	gost89.n[1] ^= gost89Pass(gost89.n[0] + gost89.k[1]);
	gost89.n[0] ^= gost89Pass(gost89.n[1] + gost89.k[0]);

	outres[0] = (uint8_t)(gost89.n[1] & 0xff);
	outres[1] = (uint8_t)((gost89.n[1] >> 8) & 0xff);
	outres[2] = (uint8_t)((gost89.n[1] >> 16) & 0xff);
	outres[3]= (uint8_t)(gost89.n[1] >> 24);
	outres[4] = (uint8_t)(gost89.n[0] & 0xff);
	outres[5] = (uint8_t)((gost89.n[0] >> 8) & 0xff);
	outres[6] = (uint8_t)((gost89.n[0] >> 16) & 0xff);
	outres[7] = (uint8_t)(gost89.n[0] >> 24);
}

uint8_t * gost89Crypt64_CFB(uint8_t * iv, uint8_t * plain) {
	uint8_t gamma[8];
	uint8_t * outres = malloc(8);
	memcpy(gamma, gost89.gamma, 8);
	gost89Crypt64(iv, gamma);
	for (int i = 0; i < 8; i++)
	{
		outres[i] = (plain[i] ^ gamma[i]);
		iv[i] = outres[i];
	}
	return outres;
}

uint8_t * gost89Decrypt64_CFB(uint8_t * iv, uint8_t * crypted) {
	uint8_t gamma[8];
	uint8_t * outres = malloc(8);
	memcpy(gamma, gost89.gamma, 8);
	gost89Crypt64(iv, gamma);
	for (int i = 0; i < 8; i++)
	{
		outres[i] = (crypted[i] ^ gamma[i]);
		iv[i] = outres[i];
	}
	return outres;
}

uint8_t * gost89Crypt_CFB(uint8_t * iv, uint8_t * plain, size_t plainlen) {
	size_t blocks = ceil(plainlen / 8);
	if (blocks == 0)
	{
		return 0;
	}
	uint8_t * resarray = malloc(blocks * 8);
	for (int i = blocks-1; i >= 0; i--)
	{
		uint8_t * restblock = malloc(8);
		memcpy(restblock, &plain[i * 8], plainlen - i * 8 >= 8 ? 8 : plainlen - i * 8);
		uint8_t * cryptedres = gost89Crypt64_CFB(iv, restblock);
		memcpy(&resarray[i * 8], &cryptedres[0], 8);
		free(restblock);
		free(cryptedres);
	}
	return resarray;
}

uint8_t * gost89Decrypt_CFB(uint8_t * iv, uint8_t * crypted, size_t cryptedlen) {
	size_t blocks = ceil(cryptedlen / 8);
	if (blocks == 0)
	{
		return 0;
	}
	uint8_t * resarray = malloc(blocks * 8);
	for (int i = blocks-1; i >= 0; i--)
	{
		uint8_t * restblock = malloc(8);
		memcpy(restblock, &crypted[i * 8], cryptedlen - i * 8 >= 8 ? 8 : cryptedlen - i * 8);
		uint8_t * plainres = gost89Decrypt64_CFB(iv, restblock);
		memcpy(&resarray[i * 8], &plainres[0], 8);
		free(restblock);
		free(plainres);
	}
	return resarray;
}

uint8_t * gost89Crypt(uint8_t * iv, uint8_t * plain, size_t plainlen) {
	size_t blocks = ceil(plainlen / 8);
	if (blocks == 0)
	{
		return 0;
	}
	uint8_t * resarray = malloc(blocks * 8);
	for (int i = blocks-1; i >= 0; i--)
	{
		uint8_t * restblock = malloc(8);
		memcpy(restblock, &plain[i * 8], plainlen - i * 8 >= 8 ? 8 : plainlen - i * 8);
		uint8_t * cryptedres = malloc(8);
		gost89Crypt64(&restblock[0], &cryptedres[0]);
		memcpy(&resarray[i * 8], &cryptedres[0], 8);
		free(restblock);
		free(cryptedres);
	}
	return resarray;
}

uint8_t * gost89Decrypt(uint8_t * crypted, size_t cryptedlen) {
	size_t blocks = ceil(cryptedlen / 8);
	if (blocks == 0)
	{
		return 0;
	}
	uint8_t * resarray = malloc(blocks * 8);
	for (int i = blocks-1; i >= 0; i--)
	{
		uint8_t * restblock = malloc(8);
		memcpy(restblock, &crypted[i * 8], cryptedlen - i * 8 >= 8 ? 8 : cryptedlen - i * 8);
		uint8_t * plainres = malloc(8);
		gost89Decrypt64(&restblock[0], &plainres[0]);
		memcpy(&resarray[i * 8], &plainres[0], 8);
		free(restblock);
		free(plainres);
	}
	return resarray;
}

void gost89Key(uint8_t * k) {
	size_t j = 0;
	for (size_t i = 0; i < 8; i++)
	{
		gost89.k[i] = (uint32_t)((uint32_t)k[j] | (((uint32_t)k[j + 1] << 8) | ((uint32_t)k[j + 2] << 16) | ((uint32_t)k[j + 3] << 24)));
		j += 4;
	}
}

size_t gost89MacOut(uint8_t * buf, int nbits, uint8_t * outres) {
	size_t nbytes = nbits >> 3;
	size_t rembits = nbits & 7;
	uint8_t mask = (rembits > 1) ? ((uint8_t)(1 < rembits) - 1) : 0;

	for (int i = 0; i < nbytes; i++)
	{
		outres[i] = buf[i];
	}
	if (rembits > 1)
	{
		outres[nbytes] = (buf[nbytes] & mask);
	}
	return nbytes;
}

void gost89Mac64(uint8_t * datablock, uint8_t * outres) {
	uint32_t n[2];
	memcpy(n, gost89.n, sizeof(uint32_t) * 2);
	for (int i = 0; i < 8; i++)
	{
		outres[i] ^= datablock[i];
	}
	n[0] = (datablock[0] | ((uint32_t)datablock[1] << 8) | ((uint32_t)datablock[2] << 16) | ((uint32_t)datablock[3] << 24));
	n[1] = (datablock[4] | ((uint32_t)datablock[5] << 8) | ((uint32_t)datablock[6] << 16) | ((uint32_t)datablock[7] << 24));

	n[1] ^= gost89Pass(n[0] + gost89.k[0]);
	n[0] ^= gost89Pass(n[1] + gost89.k[1]);
	n[1] ^= gost89Pass(n[0] + gost89.k[2]);
	n[0] ^= gost89Pass(n[1] + gost89.k[3]);
	n[1] ^= gost89Pass(n[0] + gost89.k[4]);
	n[0] ^= gost89Pass(n[1] + gost89.k[5]);
	n[1] ^= gost89Pass(n[0] + gost89.k[6]);
	n[0] ^= gost89Pass(n[1] + gost89.k[7]);

	n[1] ^= gost89Pass(n[0] + gost89.k[0]);
	n[0] ^= gost89Pass(n[1] + gost89.k[1]);
	n[1] ^= gost89Pass(n[0] + gost89.k[2]);
	n[0] ^= gost89Pass(n[1] + gost89.k[3]);
	n[1] ^= gost89Pass(n[0] + gost89.k[4]);
	n[0] ^= gost89Pass(n[1] + gost89.k[5]);
	n[1] ^= gost89Pass(n[0] + gost89.k[6]);
	n[0] ^= gost89Pass(n[1] + gost89.k[7]);

	outres[0] = (n[0] & 0xff);
	outres[1] = ((n[0] >> 8) & 0xff);
	outres[2] = ((n[0] >> 16) & 0xff);
	outres[3] = (n[0] >> 24);
	outres[4] = (n[1] & 0xff);
	outres[5] = ((n[1] >> 8) & 0xff);
	outres[6] = ((n[1] >> 16) & 0xff);
	outres[7] = (n[1] >> 24);
}

void gost89Mac(uint8_t * data, size_t datalen, uint8_t * outres) {
	uint8_t buf[8];
	for (int i = 0; i < datalen; i += 8)
	{
		uint8_t * restplain = malloc(datalen - (i * 8));
		memcpy(restplain, &data[i * 8], datalen - (i * 8));
		uint8_t * restblock = malloc(8);
		memcpy(restblock, restplain, 8);
		gost89Mac64(restblock, buf);
		free(restplain);
		free(restblock);
	}
	gost89MacOut(buf, datalen, outres);
}

void Gost89HashSwapBytes(uint8_t * w, uint8_t * k) {
	for (uint8_t i = 0; i < 4; i++)
	{
		for (uint8_t j = 0; j < 8; j++)
		{
			k[i + 4 * j] = w[8 * i + j];
		}
	}
}

void Gost89HashCircle_XOR8(uint8_t * w, uint8_t * k) {
	for (int i = 0; i < 8; i++)
	{
		gost89Hash_C8Buf[i] = w[i];
	}
	for (int i = 0; i < 24; i++)
	{
		k[i] = w[i + 8];
	}
	for (int i = 0; i < 8; i++)
	{
		k[i + 24] = (gost89Hash_C8Buf[i] ^ k[i]);
	}
}

void Gost89HashTransform_3(uint8_t *  data) {
	int32_t t16 = (int32_t)(data[0] ^ data[2] ^ data[4] ^ data[6] ^ data[24] ^ data[30]) | ((data[1] ^ data[3] ^ data[5] ^ data[7] ^ data[25] ^ data[31]) << 8);
	for (size_t i = 0; i < 30; i++)
	{
		data[i] = data[i + 2];
	}
	data[30] = (t16 & 0xff);
	data[31] = t16 >> 8;
}

int32_t Gost89HashAddBlocks(size_t n, uint8_t * left, uint8_t * right) {
	gost89Hash_AB2[2] = 0;
	gost89Hash_AB2[3] = 0;

	for (size_t i = 0; i < n; i++)
	{
		gost89Hash_AB2[0] = left[i];
		gost89Hash_AB2[1] = right[i];
		gost89Hash_AB2[2] = gost89Hash_AB2[0] + gost89Hash_AB2[1] + gost89Hash_AB2[3];
		left[i] = (gost89Hash_AB2[2] & 0xFF);
		gost89Hash_AB2[3] = gost89Hash_AB2[2] >> 8;
	}
	return gost89Hash_AB2[3];
}

void Gost89HashXorBlocks(uint8_t * ret, uint8_t * a, size_t alen, uint8_t * b) {
	for (size_t i = 0; i < alen; i++)
	{
		ret[i] = (a[i] ^ b[i]);
	}
}

void Gost89HashStep(uint8_t * h, uint8_t  * m) {
	Gost89HashXorBlocks(gost89Hash_W, h, 32, m);
	Gost89HashSwapBytes(gost89Hash_W, gost89_Key);
	gost89Key(gost89_Key);
	gost89Crypt64(h, gost89Hash__S);
	Gost89HashCircle_XOR8(h, gost89Hash_U);
	Gost89HashCircle_XOR8(m, gost89Hash_V);
	Gost89HashCircle_XOR8(gost89Hash_V, gost89Hash_V);
	Gost89HashXorBlocks(gost89Hash_W, gost89Hash_U, 32, gost89Hash_V);
	Gost89HashSwapBytes(gost89Hash_W, gost89_Key);
	gost89Key(gost89_Key);
	gost89Crypt64(&h[8], &gost89Hash__S[8]);
	Gost89HashCircle_XOR8(gost89Hash_U, gost89Hash_U);
	gost89Hash_U[31] = ~gost89Hash_U[31]; gost89Hash_U[29] = ~gost89Hash_U[29]; gost89Hash_U[28] = ~gost89Hash_U[28]; gost89Hash_U[24] = ~gost89Hash_U[24];
	gost89Hash_U[23] = ~gost89Hash_U[23]; gost89Hash_U[20] = ~gost89Hash_U[20]; gost89Hash_U[18] = ~gost89Hash_U[18]; gost89Hash_U[17] = ~gost89Hash_U[17];
	gost89Hash_U[14] = ~gost89Hash_U[14]; gost89Hash_U[12] = ~gost89Hash_U[12]; gost89Hash_U[10] = ~gost89Hash_U[10]; gost89Hash_U[8] = ~gost89Hash_U[8];
	gost89Hash_U[7] = ~gost89Hash_U[7]; gost89Hash_U[5] = ~gost89Hash_U[5]; gost89Hash_U[3] = ~gost89Hash_U[3]; gost89Hash_U[1] = ~gost89Hash_U[1];
	Gost89HashCircle_XOR8(gost89Hash_V, gost89Hash_V);
	Gost89HashCircle_XOR8(gost89Hash_V, gost89Hash_V);
	Gost89HashXorBlocks(gost89Hash_W, gost89Hash_U, 32, gost89Hash_V);
	Gost89HashSwapBytes(gost89Hash_W, gost89_Key);
	gost89Key(gost89_Key);
	gost89Crypt64(&h[16], &gost89Hash__S[16]);
	Gost89HashCircle_XOR8(gost89Hash_U, gost89Hash_U);
	Gost89HashCircle_XOR8(gost89Hash_V, gost89Hash_V);
	Gost89HashCircle_XOR8(gost89Hash_V, gost89Hash_V);
	Gost89HashXorBlocks(gost89Hash_W, gost89Hash_U, 32, gost89Hash_V);
	Gost89HashSwapBytes(gost89Hash_W, gost89_Key);
	gost89Key(gost89_Key);
	gost89Crypt64(&h[24], &gost89Hash__S[24]);
	for (size_t i = 0; i < 12; i++)
	{
		Gost89HashTransform_3(gost89Hash__S);
	}
	Gost89HashXorBlocks(gost89Hash__S, gost89Hash__S, 32, m);
	Gost89HashTransform_3(gost89Hash__S);
	Gost89HashXorBlocks(gost89Hash__S, gost89Hash__S, 32, h);
	for (size_t i = 0; i < 61; i++)
	{
		Gost89HashTransform_3(gost89Hash__S);
	}
	for (size_t i = 0; i < 32; i++)
	{
		gost89Hash_Value[i] = gost89Hash__S[i];
	}
}

void Gost89HashUpdate(uint8_t * block, size_t blocklen) {
	if (gost89Hash_Leftlength > 0)
	{
		gost89Hash_Leftlength = gost89Hash_Leftlength + blocklen;
		memcpy(&gost89Hash_Left[gost89Hash_Leftlength], block, blocklen);
	}
	uint8_t block32[blocklen];
	memcpy(block32, block, blocklen);
	uint8_t processed = 0;
	while (blocklen - processed > 31)
	{
		Gost89HashStep(gost89Hash_Value, block32);
		Gost89HashAddBlocks(32, gost89Hash_S, block32);
		processed += 32;
		memcpy(&block32[0], &block[processed], 32);
	}
	gost89HashLength += processed;

	if (blocklen - processed > 0)
	{
		gost89Hash_Leftlength = blocklen - processed;
		memcpy(gost89Hash_Left, &block32[0], blocklen - processed);
	}
}

void Gost89HashUpdate32(uint8_t * block32) {
	Gost89HashStep(gost89Hash_Value, block32);
	Gost89HashAddBlocks(32, gost89Hash_S, block32);
	gost89HashLength += 32;
}

void Gost89HashFinish() {
	size_t fin_len = gost89HashLength;
	size_t idx = 0;
	if (gost89Hash_Leftlength > 0)
	{
		for (idx = 0; idx < gost89Hash_Leftlength; idx++)
		{
			gost89Hash_Buf[idx] = gost89Hash_Left[idx];
		}
		Gost89HashStep(gost89Hash_Value, gost89Hash_Buf);
		Gost89HashAddBlocks(32, gost89Hash_S, gost89Hash_Buf);
		fin_len += gost89Hash_Leftlength;
		gost89Hash_Leftlength = 0;

		for (idx = 0; idx < 32; idx++)
		{
			gost89Hash_Buf[idx] = 0;
		}
	}

	fin_len <<= 3;
	idx = 0;
	while (fin_len > 0)
	{
		gost89Hash_Buf[idx++] = (fin_len & 0xFF);
		fin_len >>= 8;
	}
	Gost89HashStep(gost89Hash_Value, gost89Hash_Buf);
	Gost89HashStep(gost89Hash_Value, gost89Hash_S);
}

void Gost89HashReset() {
	for (int idx = 0; idx < 32; idx++)
	{
		gost89Hash_Value[idx] = 0;
		gost89Hash_S[idx] = 0;
	}
	gost89Hash_Leftlength = 0;
	gost89HashLength = 0;
}

void Gost89Dumb_KDF(uint8_t * input, size_t inputlen, size_t n_passes) {
	Gost89HashUpdate(input, inputlen);
	Gost89HashFinish();
	uint8_t * hash = malloc(32);
	n_passes--;
	while (n_passes-- > 0)
	{
		memcpy(hash, &gost89Hash_Value[0], 32);
		Gost89HashReset();
		Gost89HashUpdate32(&hash[0]);
		Gost89HashFinish();
		if (n_passes % 10 == 0) 
		{
			EXT_UART_Transmit("*");
			PORTD ^= 0x40;
		}
	}
	free(hash);
}

uint8_t * Gost89PB_KDF(uint8_t * input, size_t inputlen, uint8_t * salt, size_t saltlen, size_t iterations) {
	uint8_t * key = malloc(32);
	uint8_t * pw_pad36 = malloc(32);
	uint8_t * pw_pad5C = malloc(32);
	uint8_t ins[4] = { 0,0,0,1 };
	for (size_t k = 0; k < 32; k++)
	{
		pw_pad36[k] = 0x36;
		pw_pad5C[k] = 0x5C;
	}
	for (size_t k=0; k < inputlen; k++) {
		pw_pad36[k] ^= input[k];
		pw_pad5C[k] ^= input[k];
	}
	Gost89HashUpdate(pw_pad36, 32);
	Gost89HashUpdate(salt, saltlen);
	Gost89HashUpdate(ins, 32);
	Gost89HashFinish();

	Gost89HashReset();

	Gost89HashUpdate32(pw_pad5C);
	Gost89HashUpdate32(gost89Hash_Value);
	Gost89HashFinish();

	iterations--;

	for (size_t k = 0; k < 32; k++)
	{
		key[k] = gost89Hash_Value[k];
	}

	while (iterations-- > 0)
	{
		Gost89HashReset();
		Gost89HashUpdate32(pw_pad36);
		Gost89HashUpdate32(gost89Hash_Value);
		Gost89HashFinish();

		Gost89HashReset();
		Gost89HashUpdate32(pw_pad5C);
		Gost89HashUpdate32(gost89Hash_Value);
		Gost89HashFinish();

		for (size_t k = 0; k < 32; k++)
		{
			key[k] ^= gost89Hash_Value[k];
		}
	}
	free(pw_pad36);
	free(pw_pad5C);
	return key;
}

void Gost89ConvertPassword(uint8_t * pw, size_t pwlen) {
	Gost89Dumb_KDF(pw, pwlen, 10000);
}

uint8_t bkey[32] = {
	0x34, 0x7C, 0x1B, 0x89, 0x7A, 0x38, 0x37, 0x27, 0xF0, 0xB8, 0x4A, 0xE8,
	0x4D, 0x36, 0x51, 0x8F, 0xC1, 0x72, 0xDE, 0xBC, 0xE5, 0xC4, 0x62, 0x65,
	0x0B, 0xCA, 0x9A, 0x72, 0x68, 0x15, 0xA9, 0x80
};

uint8_t * DecodeData(uint8_t * crypted, size_t cryptlen, uint8_t * pw, size_t pwlen) {
	ZeroAll();
	gost89_init(0);
	Gost89Dumb_KDF(pw, pwlen, 10000);
	PrintDebugByteArray(&gost89Hash_Value[0], 32);
	//gost89_init(0);
	gost89Key(bkey);
	uint8_t * tmpres = gost89Decrypt(crypted, cryptlen);
	PrintDebugByteArray(tmpres, 912);
	return gost89Hash_Value;
}