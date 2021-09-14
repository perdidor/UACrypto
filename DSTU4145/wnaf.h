/*
 * wnaf.h
 *
 * Created: 08.09.2021 13:46:07
 *  Author: root
 */ 

#ifndef WNAF_H_
#define WNAF_H_

#include "dstu_types.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

size_t windowNaf(uint32_t width, field_t * field, int * resarray, curve_t * curve);
size_t getWindowSize(uint32_t bits);


#endif /* WNAF_H_ */