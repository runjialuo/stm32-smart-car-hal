#include "Servo.h"
#include "PWM.h"
#include "main.h"
#include "tim.h"

/************************************************** *************
@brief
引脚配置：
htim1.Instance = TIM1;
htim1.Init.Prescaler = 71;                                              //预分频（71+1）=72
htim1.Init.CounterMode = TIM_COUNTERMODE_UP;                            //上升沿触发
htim1.Init.Period = 19999;                                              //ARR(自动重装值)
htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;                      //时钟分频（72MHz/DIVx）
htim1.Init.RepetitionCounter = 0;                                       //重复计数器，发生n+1次计数器溢出后才会发生更新事件
htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;           //使能自动重装计数器

TIM1 挂在 APB2 上，APB2 预分频 = 1 → TIM1CLK = 72MHz
 *    PSC = 71    → 计数频率 = 72MHz / (71+1) = 1MHz   ⇒ 1 个数 = 1µs
 *    ARR = 19999 → PWM 周期 = 20000µs = 20ms           ⇒ 50Hz（舵机标准）
 *
 *  ⇒ CCR 的数值"直接等于"高电平脉宽，单位 µs
****************************************************************/

static float curAngle = 90.0f;

/*
@brief设置舵机角度
*/
void Servo_SetAngle(float angle)
{
    if(angle < 0.0f)  angle = 0.0f;
    if(angle > 180.f) angle = 180.0f;

    curAngle = angle;

    /*****************************************************
    SG90 的协议规定：角度与脉宽成线性关系，两个端点是确定的。
    0°   → 500 μs
    180° → 2500 μs(对应了2ms为舵机最适周期)
    斜率k=（2500 - 500）/（180-0）
    截距b=500
    占空比=CCR/ARR
    对应CCR计数器比较值为：(500.0f + Angle *(2000.0f / 180.0f))
    *************************************************************/
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t)(500.0f + angle*((2500.0f-500.0f)/180.f)));
}
/*
@brief舵机初始化
*/
void Servo_Init()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    Servo_SetAngle(90.0f);
}

/*
@brief返回当前舵机角度
*/
float Servo_GetAngle(void)
{
    return curAngle;
}