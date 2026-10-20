#include "PWM.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

/*==============================================================
 *  TIM2 参数（CubeMX 里配的，见 Core/Src/tim.c → MX_TIM2_Init）
 *
 *  TIM2 挂在 APB1 上：
 *    APB1 预分频 = 2 → PCLK1 = 36MHz
 *    但"APB1 预分频 ≠ 1"时，硬件会把定时器时钟自动 ×2 补偿
 *    ⇒ TIM2CLK = 36MHz × 2 = 72MHz（和 SYSCLK 一样）
 *
 *  PSC = 35  → 计数频率 = 72MHz / (35+1) = 2MHz
 *  ARR = 99  → PWM 频率 = 2MHz / (99+1) = 20kHz
 *              （20kHz 超出人耳听觉上限，电机不会啸叫）
 *
 *  占空比 = CCR / (ARR+1) = CCR / 100
 *  ⇒ 传进来的 Compare 直接就是百分比：0 ~ 100
 *=============================================================*/

#define PWM_MAX_COMPARE 100

void PWM_Init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);   //CH2->PA1->PWMA->左轮
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);   //CH3->PA2->PWMB->右轮
}

/*
*@brief设置左轮的PWM占空比
*/
void PWM_SetCompare2(uint16_t Compare)
{
    if(Compare > PWM_MAX_COMPARE) Compare = PWM_MAX_COMPARE;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, Compare);
}
/*
*@brief设置右轮的PWM占空比
*/
void PWM_SetCompare3(uint16_t Compare)
{
    if(Compare > PWM_MAX_COMPARE) Compare = PWM_MAX_COMPARE;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, Compare);
}

