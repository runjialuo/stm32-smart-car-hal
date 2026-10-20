#ifndef PWM_H
#define PWM_H

#include "stm32f1xx_hal.h"

void PWM_Init(void);
void PWM_SetCompare2(uint16_t Compare);     // 左轮 PWMA = PA1 = TIM2_CH2
void PWM_SetCompare3(uint16_t Compare);     // 右轮 PWMB = PA2 = TIM2_CH3

#endif