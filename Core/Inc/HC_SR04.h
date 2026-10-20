#ifndef HC_SR04_H
#define HC_SR04_H

#include "main.h"

void HC_SR04_Init(void);

float Get_Distance_cm(void);

uint8_t Can_Pass(float distance);

int8_t Scan_Way(void);

void Auto_Run(float distance);

#endif
