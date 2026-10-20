#ifndef SERIAL_H
#define SERIAL_H

#include "main.h"
/*没收到数据的垃圾值*/
#define SERIAL_RX_NONE 0xFF

extern volatile uint8_t Serial_RxData;

void Serial_Init();

void Serial_SendByte(uint8_t Byte);

void Serial_SendArray(uint8_t *Array, uint16_t Length);

void Serial_SendString(char *String);

void Serial_SendNumber(uint32_t Number);

void Serial_Printf(char *format, ...);

#endif
