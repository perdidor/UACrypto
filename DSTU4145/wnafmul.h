/*
 * wnafmul.h
 *
 * Created: 08.09.2021 14:34:17
 *  Author: root
 */ 


#ifndef WNAFMUL_H_
#define WNAFMUL_H_

#include "wnaf.h"
#include "dstu_types.h"
#include "Point.h"
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

uint8_t bitLengths[256];

void PointPrecomp(point_t * point, curve_t * curve, size_t width);



#endif /* WNAFMUL_H_ */