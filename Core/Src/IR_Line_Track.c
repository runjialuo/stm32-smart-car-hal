#include "IR_Line_Track.h"
#include "Delay.h"
#include "main.h"
#include "Car.h"

/*==========引脚配置==========
Configure GPIO pins : IR_X4_Pin IR_X3_Pin IR_X2_Pin IR_X1_Pin 
  GPIO_InitStruct.Pin = IR_X4_Pin|IR_X3_Pin|IR_X2_Pin|IR_X1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
*/

/*
X1----2.8cm----X2-0.7cm-X3----2.8cm----X4,
==要以小车得中心为中心轴（向量的x=0，y=0处），则计算出最小间距为0.7cm==    
===|----------------|----|----|------------------|
=-3.15============-0.35==0==0.35===============3.15
*/

#define IR_BASE_Speed  40.0f       //红外模块基础寻速
#define Turn_Gain      25.0f       //偏差,(本质PID算法中的Kp)
#define Turn_Speed     60          //出现三路以上压线时的急转速
#define LOST_TICK      10          //连续10圈找不到线->判定为终点，停车
#define SEEK_Speed     50          //丢线后寻找新路线的速度
#define BLACK_STOP     12
#define Speed_Limit    100.0f

static const float Weight[4] = {-3.15, -0.35, 0.35, 3.15};    //四个红外探测器的权重,单位1cm
static float lastError = 0.0f;      //上一次有效偏差
static uint8_t lostCnt = 0;         //上一次丢线的次数
static uint8_t IR_State = 0;        //IR探测出的结果
static uint8_t BlackCnt = 0;        //停留黑色的次数

/*读取路况：低电平=压倒黑线*/
uint8_t IR_Read(void)
{
  uint8_t State = 0;
  if(HAL_GPIO_ReadPin(IR_X1_GPIO_Port, IR_X1_Pin)==GPIO_PIN_RESET) State |= 0x01; //0001
  if(HAL_GPIO_ReadPin(IR_X2_GPIO_Port, IR_X2_Pin)==GPIO_PIN_RESET) State |= 0x02; //0010
  if(HAL_GPIO_ReadPin(IR_X3_GPIO_Port, IR_X3_Pin)==GPIO_PIN_RESET) State |= 0x04; //0100
  if(HAL_GPIO_ReadPin(IR_X4_GPIO_Port, IR_X4_Pin)==GPIO_PIN_RESET) State |= 0x08; //1000

  return State;
}
/*红外循迹*/
void IR_Line_follow()
{
  float Sum = 0.0f, Error, Left, Right;
  uint8_t i,Count=0;
  IR_State = IR_Read();

  for(i=0; i<4; i++)
  {
    if(IR_State&(1<<i))
    {
      Sum+=Weight[i];
      Count++;
    }
  }

  /*3线压路，遇到横线*/
  if(Count>=3)
  {
    lostCnt = 0;
    if(++BlackCnt >= BLACK_STOP){
      BlackCnt = BLACK_STOP;
      Car_Stop();
      return ;
    }
    /*短暂黑区*/
    Car_SetSpeed(IR_BASE_Speed, IR_BASE_Speed);
    return ;
  }

  /*完全丢线*/
  if(Count == 0){
    BlackCnt = 0;
    if(++lostCnt > LOST_TICK){
      Car_Stop();
      return ;
    }
    else{
      if(lastError >= 0.0f) Car_SetSpeed(SEEK_Speed, -SEEK_Speed);
      else                  Car_SetSpeed(-SEEK_Speed, SEEK_Speed);
    }
    return ;
  }

  lostCnt = 0;
  BlackCnt = 0;
  Error = Sum/Count;
  lastError = Error;

  Left = IR_BASE_Speed+Error*Turn_Gain;
  Right = IR_BASE_Speed-Error*Turn_Gain;

  if(Left > Speed_Limit)    Left=Speed_Limit;
  if(Left < -Speed_Limit)   Left=-Speed_Limit;
  if(Right > Speed_Limit)   Right=Speed_Limit;
  if(Right < -Speed_Limit)  Right=-Speed_Limit;

  Car_SetSpeed((int8_t)Left, (int8_t)Right);
}
