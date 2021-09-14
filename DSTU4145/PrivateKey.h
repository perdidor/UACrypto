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

typedef struct {
	uint8_t type;
	uint8_t ks_type;
	uint8_t bp[33];
	uint8_t param_b[33];
	uint8_t p_ks_rawdata_b[33];
	uint32_t * param_a;
	uint32_t * order;
	uint32_t * param_m;
	uint32_t * trinominal_value;
	uint32_t * pentanominal_k1;
	uint32_t * pentanominal_k2;
	uint32_t * pentanominal_k3;
	uint32_t trinominal_value_len;
	uint32_t pentanominal_k1_len;
	uint32_t pentanominal_k2_len;
	uint32_t pentanominal_k3_len;
	uint32_t param_m_len;
	uint32_t order_len;
	uint32_t param_a_len;
	uint32_t bp_len;
	uint32_t param_b_len;
	uint32_t p_ks_rawdata_len;
} dstuprivkey_t;

uint8_t DSTUPrivKeyRawData[127];

#endif /* PRIVATEKEY_H_ */