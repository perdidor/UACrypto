/*
 * ConvertHelper.c
 *
 * Created: 12.09.2021 07:26:53
 *  Author: root
 */ 
#include "dstu_types.h"
#include "ConvertHelper.h"
//#include "Curve.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

uint8_t ConvertChar2Int(char input) {
	if(input >= '0' && input <= '9')
	return input - '0';
	if(input >= 'A' && input <= 'F')
	return input - 'A' + 10;
	if(input >= 'a' && input <= 'f')
	return input - 'a' + 10;
	return 0x00;
}

size_t ConvertHex2Bin(char * src, uint32_t * target) {
	size_t reslen = 0;
	while(*src && src[1])
	{
		*(target++) = ConvertChar2Int(*src)*16 + ConvertChar2Int(src[1]);
		src += 2;
		reslen++;
	}
	return reslen;
}

uint8_t * InvertByteArray(uint8_t * indata, size_t len) {
	uint8_t * res = malloc(len + 1);
	uint8_t cr = 0;
	for (size_t i = 0; i < len; i++)
	{
		cr = indata[i];
		cr = ((cr >> 7) | ((cr >> 5) &  2) | ((cr >> 3) &  4) | ((cr >> 1) & 8) | ((cr << 1) & 16) | ((cr << 3) & 32) | ((cr << 5) & 64) | ((cr << 7) & 128));
		res[len - i] = cr;
	}
	return res;
}