/*
 * notaxTemplate.h
 *
 * Created: 16.09.2021 09:24:30
 *  Author: root
 */ 


#ifndef NOTAXTEMPLATE_H_
#define NOTAXTEMPLATE_H_

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

const uint8_t NoTaxTemplate1_63[63];
const uint8_t NoTaxTemplate2_2458[451];
const uint8_t NoTaxTemplate3_30[30];
const uint8_t NoTaxTemplate4_81[81];


const size_t NoTaxTotalLengthInitValue;
const size_t NoTaxElem1LengthInitValue;
const size_t NoTaxElem2LengthInitValue;
const size_t NoTaxElem3LengthInitValue;

uint8_t dtspreamble[25];
uint8_t dtspreamble2[43];
uint8_t dtspreamble3[15];
uint8_t dts_timestamp[15];

uint8_t DataToSign[512];
size_t DataToSignLength;
size_t NoTaxTemplate2_certv2offset;

void NoTaxPrepareTemplate(size_t rawDataLength, uint8_t * timestamp, uint8_t * hashvalue, uint8_t * signature);
void NoTaxPrepareDataToSign(uint8_t * hashvalue);


#endif /* NOTAXTEMPLATE_H_ */