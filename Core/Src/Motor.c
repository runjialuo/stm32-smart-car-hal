#include "Motor.h"
#include "PWM.h"
#include "main.h"

/*==============================================================
 *  TB6612FNG 接线（引脚宏在 Core/Inc/main.h 里，CubeMX 自动生成）
 *
 *   左轮：PWMA = PA1 (TIM2_CH2)    AIN1 = PA5    AIN2 = PA4
 *   右轮：PWMB = PA2 (TIM2_CH3)    BIN1 = PA6    BIN2 = PA7
 *
 *  ⚠️ 方向脚（AIN1/AIN2/BIN1/BIN2）的 GPIO 初始化，
 *     已经在 MX_GPIO_Init() 里做完了（推挽输出 + 上电默认低电平）。
 *     所以 HAL 版【不需要】再写 Motor_Left_Init / Motor_Right_Init，
 *     这是相比 SPL 版省掉的一段代码。
 *
 *  方向真值表（以左轮为例）：
 *      AIN1=0, AIN2=1  → 正转（前进）
 *      AIN1=1, AIN2=0  → 反转（后退）
 *      PWM 占空比      → 决定转速
 *=============================================================*/

 /*
 @brief电机初始化
 */
void Motor_Init(void)
{
    PWM_Init();
}

/*
@brief 设置左轮速度
*/
void Motor_Left_SetSpeed(int8_t Speed)
{
    /*正转：AIN1=0,AIN2=1*/
    if(Speed >= 0)          
    {
        HAL_GPIO_WritePin(tb6612fng_AIN1_GPIO_Port, tb6612fng_AIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(tb6612fng_AIN2_GPIO_Port, tb6612fng_AIN2_Pin, GPIO_PIN_SET);
        PWM_SetCompare2((uint16_t)Speed);
    }
    /*反转，AIN1=1,AIN2=0*/
    else{       
        HAL_GPIO_WritePin(tb6612fng_AIN1_GPIO_Port, tb6612fng_AIN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(tb6612fng_AIN2_GPIO_Port, tb6612fng_AIN2_Pin, GPIO_PIN_RESET);
        PWM_SetCompare2(-Speed);
    }
}

/*
@brief 设置右轮速度
*/
void Motor_Right_SetSpeed(int8_t Speed)
{
    /*正转：BIN1=0，BIN2=1*/
    if(Speed >= 0)
    {
        HAL_GPIO_WritePin(tb6612fng_BIN1_GPIO_Port, tb6612fng_BIN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(tb6612fng_BIN2_GPIO_Port, tb6612fng_BIN2_Pin, GPIO_PIN_SET);
        PWM_SetCompare3(Speed);
    }
    /*反转：BIN1=1,BIN2=0*/
    else
    {
        HAL_GPIO_WritePin(tb6612fng_BIN1_GPIO_Port, tb6612fng_BIN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(tb6612fng_BIN2_GPIO_Port, tb6612fng_BIN2_Pin, GPIO_PIN_RESET);
        PWM_SetCompare3(-Speed);
    }
}

