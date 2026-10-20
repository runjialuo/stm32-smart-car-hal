#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f1xx_hal.h"

void Motor_Init(void);
void Motor_Left_SetSpeed(int8_t Speed);
void Motor_Right_SetSpeed(int8_t Speed);

#endif