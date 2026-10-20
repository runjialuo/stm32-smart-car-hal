#ifndef SERVO_H
#define SERVO_H

#include "main.h"

void Servo_Init(void);

void Servo_SetAngle(float angle);

float Servo_GetAngle(void);

#endif