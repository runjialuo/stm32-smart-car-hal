#include "BlueToothCtrl.h"
#include "Car.h"
#include "Motor.h"
#include "Serial.h"

#define BT_OutCtrl_Time 250
#define BT_Connect_STATE    GPIO_PIN_RESET
static uint32_t lastCmdTick = 0;

uint8_t BT_IsConnect(void)
{
    return (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == BT_Connect_STATE);
}



void BTControl()
{
    /* 蓝牙断连保护 */
    if(!BT_IsConnect())
    {
        Car_Stop();
        return;
    }
    else
    {
        uint8_t cmd = Serial_RxData;

        Serial_RxData = SERIAL_RX_NONE;
        if(cmd != SERIAL_RX_NONE)
        {
            lastCmdTick = HAL_GetTick();
            switch(cmd)
            {
                case '0':
                    Car_Stop();
                    break;
                case '1':
                    Car_Forward();
                    break;
                case '2':
                    Car_Back();
                    break;
                case '3':
                    Car_TurnLeft();
                    break;
                case '4':
                    Car_TurnRight();
                    break;
                default:
                    break;
            }
        }
    }

}
