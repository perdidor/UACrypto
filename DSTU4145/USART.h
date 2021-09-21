/*
 * USART.h
 *
 * Created: 31.08.2021 10:03:43
 *  Author: root
 */ 


#ifndef USART_H_
#define USART_H_

void EXT_UART_Setup(void);
void EXT_UART_Transmit(char * data);
void EXT_CRLF(void);
void PrintDebugByteArray(uint8_t * outarr, size_t arrlen);
void EXT_UART_TransmitRawData(uint8_t * outarr, size_t arrlen);
void PrintDebugUInt32Array(uint32_t * arr, uint32_t arrlen);


#endif /* USART_H_ */