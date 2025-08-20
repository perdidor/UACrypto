/*
 * PubKey.h
 *
 * Created: 04.10.2021 09:40:13
 *  Author: root
 */ 


#ifndef PUBKEY_H_
#define PUBKEY_H_

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include "dstu_types.h"

//precomp_set_t PubKeyPreComputedPoints;

//uint32_t PubKeyPointX[9];
//uint32_t PubKeyPointY[9];

//point_t PubKeypoint;

bool VerifySignature(uint8_t * hashvalue, uint8_t * signature);



#endif /* PUBKEY_H_ */