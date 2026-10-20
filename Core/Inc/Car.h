#ifndef CAR_H
#define CAR_H

#include "Motor.h"
#include "main.h"

void Car_SetSpeed(int8_t l_Speed, int8_t r_Speed);

void Car_Stop(void);

void Car_Forward(void);

void Car_Back(void);

void Car_TurnLeft(void);

void Car_TurnRight(void);

// int8_t Car_GetLeftSpeed(void);

// int8_t Car_GetRightSpeed(void);

// void Car_Faster(int8_t *l_Speed, int8_t *r_Speed);

// void Car_Slower(int8_t *l_Speed, int8_t *r_Speed);

#endif
