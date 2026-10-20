/****************引脚配置*************************
  Configure GPIO pin : HC_SR04_Trig_Pin 
  GPIO_InitStruct.Pin = HC_SR04_Trig_Pin;           //触发引脚
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;       //推挽输出
  GPIO_InitStruct.Pull = GPIO_NOPULL;               //浮空
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;      //低速
  HAL_GPIO_Init(HC_SR04_Trig_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = HC_SR04_Echo_Pin;           //数据采集引脚
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;           //输入
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;             //下拉
  HAL_GPIO_Init(HC_SR04_Echo_GPIO_Port, &GPIO_InitStruct);
*********************************************/

#include "HC_SR04.h"
#include "Motor.h"
#include "Serial.h"
#include "Servo.h"
#include "main.h"
#include "Delay.h"
#include "stm32f1xx_hal_gpio.h"
#include "tim.h"
#include "Car.h"

#define Distance_Over  (-0.1f)  //如果距离过远
#define STOP_CM         10.0f   //停止
#define GO_CM           15.0f   //前进
#define MAX_DISTANCE    999.0f
#define SCAN_RIGHT      30.0f
#define SCAN_LEFT       150.0f
#define SCAN_FRONT      90.0f
#define SCAN_Time_ms    200     //扫描时间
/*============ 避障模式可调参数 ============*/
#define Ahead_SPEED     50      // 避障前进速度（比循迹慢 → 刹车距离更短）
#define TURN_SPEED      60       // 原地转向速度
#define TURN_TIME_MS    350      // 转向持续时长，约转 45°

/*@brief超声波探测器初始化*/
void HC_SR04_Init(void)
{
    /*先置高电平，让模块处于空闲状态*/
    HAL_GPIO_WritePin(HC_SR04_Trig_GPIO_Port, HC_SR04_Trig_Pin, GPIO_PIN_RESET);
}

/*@brief获取距离函数(厘米级)
HC_SR04模块发射超声波期间Echo为高电平，接收到回传的超声波后置为低电平
*/
float Get_Distance_cm(void)
{
    uint32_t timeout = 0;
    uint16_t cnt;

    /*先给trig一个20微妙的高电平脉冲，命令模块发射*/
    HAL_GPIO_WritePin(HC_SR04_Trig_GPIO_Port, HC_SR04_Trig_Pin, GPIO_PIN_SET);
    Delay_us(20);
    HAL_GPIO_WritePin(HC_SR04_Trig_GPIO_Port, HC_SR04_Trig_Pin, GPIO_PIN_RESET);

    //等待Echo变成高电平(发射超声波)
    while(HAL_GPIO_ReadPin(HC_SR04_Echo_GPIO_Port, HC_SR04_Echo_Pin) == GPIO_PIN_RESET)
    {
        if(++timeout > 60000) return Distance_Over;
    }

    /*使用定时器计算echo的高电平时间*/
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    __HAL_TIM_ENABLE(&htim4);

    timeout = 0;
    /*等待Echo变为低电平(超声波接收完成)*/
    while(HAL_GPIO_ReadPin(HC_SR04_Echo_GPIO_Port, HC_SR04_Echo_Pin) == GPIO_PIN_SET)
    {
        if(++timeout > 60000)
        {
            __HAL_TIM_DISABLE(&htim4);
            return Distance_Over;
        }
    }
    __HAL_TIM_DISABLE(&htim4);
    /*高电平时钟脉冲数*/
    cnt = (uint16_t)__HAL_TIM_GetCounter(&htim4);

    /*
    距离换算,预分频72->1MHz，一个时钟脉冲为1微秒
    1米=100厘米，1秒=1M毫秒
    距离=脉冲数*(340)*100/1000000/2=cnt/0.034/2
    */
    float distance = cnt * 0.017f;

    if(distance > MAX_DISTANCE) return Distance_Over;

    return distance;
}

/*@brief判断距离能否通过函数*/
uint8_t Can_Pass(float distance)
{
    static uint8_t go = 1;

    if(distance < 0.0f)
    {
        go = 1;
        return go;
    }
    if(distance >= GO_CM)       go = 1;
    else if(distance < STOP_CM) go = 0;
    return go;
}

/*
@brief舵机扫描路况
返回值：-1为左，1为右
*/
int8_t Scan_Way(void)
{
    float d_left, d_right;

    /*测量左边*/
    Servo_SetAngle(SCAN_LEFT);
    Delay_ms(SCAN_Time_ms);
    d_left = Get_Distance_cm();

    /*测量右边*/
    Servo_SetAngle(SCAN_RIGHT);
    Delay_ms(SCAN_Time_ms);
    d_right = Get_Distance_cm();

    /*探头回正*/
    Servo_SetAngle(SCAN_FRONT);
    Delay_ms(SCAN_Time_ms);

    if(d_left < 0.0f) d_left = MAX_DISTANCE;
    if(d_right <0.0f) d_right = MAX_DISTANCE;

    if(d_left >= MAX_DISTANCE && d_right >= MAX_DISTANCE)   return 1;
    if(d_left < GO_CM && d_right < GO_CM)   return 0;
    
    return (d_left > d_right)? -1 : 1;
}

/*@自动避障行动*/
void Auto_Run(float distance)
{
    int8_t dir;

    if(Can_Pass(distance))
    {
        Car_SetSpeed(Ahead_SPEED, Ahead_SPEED);
    }
    else
    {
        Car_SetSpeed(0, 0);

        dir = Scan_Way();
        if(dir == 0)   dir = 1;
       Car_SetSpeed(dir*TURN_SPEED, (-1)*dir*TURN_SPEED);
        Delay_ms(TURN_TIME_MS);
    }
}


