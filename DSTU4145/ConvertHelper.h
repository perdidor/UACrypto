/*
 * ConvertHelper.h
 *
 * Created: 12.09.2021 07:26:40
 *  Author: root
 */ 


#ifndef CONVERTHELPER_H_
#define CONVERTHELPER_H_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

uint8_t ConvertChar2Int(char input);
size_t ConvertHex2Bin(char * src, uint32_t * target);


#endif /* CONVERTHELPER_H_ */