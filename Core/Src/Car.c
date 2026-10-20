#include "Car.h"
#include "Motor.h"

#define Car_MAXSpeed  100
#define Car_BaseSpeed 80
#define Car_TurnSpeed 80
#define Car_StopSpeed 0

// static int8_t cur_LeftSpeed  = 0;
// static int8_t cur_RightSpeed = 0;

void Car_SetSpeed(int8_t l_Speed, int8_t r_Speed)
{
    Motor_Left_SetSpeed(l_Speed);
    Motor_Right_SetSpeed(r_Speed);

}

void Car_Stop()
{
    Car_SetSpeed(Car_StopSpeed, Car_StopSpeed);
    // cur_LeftSpeed = 0;
    // cur_RightSpeed = 0;
}

void Car_Forward()
{
    Car_SetSpeed(Car_BaseSpeed, Car_BaseSpeed);
}

void Car_Back()
{
    Car_SetSpeed(-Car_BaseSpeed, -Car_BaseSpeed);
}

void Car_TurnLeft()
{
    Car_SetSpeed(-Car_TurnSpeed, Car_TurnSpeed);
}

void Car_TurnRight()
{
    Car_SetSpeed(Car_TurnSpeed, -Car_TurnSpeed);
}

// int8_t Car_GetLeftSpeed(void)
// {
//     return cur_LeftSpeed;
// }

// int8_t Car_GetRightSpeed(void)
// {
//     return cur_RightSpeed;
// }

// void Car_Faster(int8_t *l_Speed, int8_t *r_Speed)
// {
//     if(*l_Speed<=Car_MAXSpeed)
//     {
//         *l_Speed+=5;
//     }
//     else
//     {
//         *l_Speed = Car_MAXSpeed;
//     }
//     if(*r_Speed <= Car_MAXSpeed)
//     {
//         *r_Speed+=5;
//     }
//     else
//     {
//         *r_Speed = Car_MAXSpeed;
//     }
//     Car_SetSpeed(*l_Speed, *r_Speed);
// }

// void Car_Slower(int8_t *l_Speed, int8_t *r_Speed)
// {
//     if(*l_Speed<=Car_MAXSpeed)
//     {
//         *l_Speed-=5;
//     }
//     else
//     {
//         *l_Speed = Car_MAXSpeed;
//     }
//     if(*r_Speed <= Car_MAXSpeed)
//     {
//         *r_Speed-=5;
//     }
//     else
//     {
//         *r_Speed = Car_MAXSpeed;
//     }
//     Car_SetSpeed(*l_Speed, *r_Speed);
// }