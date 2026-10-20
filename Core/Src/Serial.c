#include "Serial.h"
#include "stm32f1xx_hal_uart.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>

volatile uint8_t Serial_RxData = SERIAL_RX_NONE;

/*
@brief串口初始化函数
*/
void Serial_Init(void)
{
    Serial_RxData = SERIAL_RX_NONE;

    HAL_UART_Receive_IT(&huart1, (uint8_t *)&Serial_RxData, 1);
}

/*
@brief串口发送一个字节
*/
void Serial_SendByte(uint8_t Byte)
{
    HAL_UART_Transmit(&huart1, &Byte, 1, 100);
}

/*@brief串口发送一个字节数组*/
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    HAL_UART_Transmit(&huart1, Array, Length, 1000);
}

/*@brief串口发送一个字符串*/
void Serial_SendString(char *String)
{
    for(uint8_t i = 0; String[i] != '\0'; i++)
    {
        Serial_SendByte(String[i]);
    }
}

/*@brief求x的y次方*/
static uint32_t Serial_Pow(uint32_t x, uint32_t y)
{
    uint32_t Result = 1;
    while(y--)
    {
        Result*=x;
    }
    return Result;
}
/*@brief发送十进制无符号数字*/
void Serial_SendNumber(uint32_t Number)
{
    uint8_t Length = 1;
    uint32_t Temp = Number;

    while(Temp/10)
    {
       Temp = Temp/10;
       Length++;
    }
    for(uint8_t i = 0; i<Length; i++)
    {
        /*例如15/10=5,5%10=5,从低到高取出最低位*/
        Serial_SendByte(Number / Serial_Pow(10, Length-i-1)%10+'0');
    }
}
/*@格式化发送*/
void Serial_Printf(char *format, ...)
{
    char String[128];
    va_list arg;

    va_start(arg, format);
    vsnprintf(String, sizeof(String), format, arg);
    va_end(arg);

    Serial_SendString(String);
}
/*=====中断部分======*/
/*
每触发一次中断，进行一次回调
接收完成回调（每收到一个字节就回调一次）
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)       /*判断触发中断的是不是串口1*/
    {
        /*串口接收中断函数*/
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&Serial_RxData, 1);
    }
}

/*串口错误回调*/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if(huart ->Instance == USART1)              //判断错误回调是不是来自串口1
    {
        __HAL_UART_CLEAR_OREFLAG(&huart1);      //清除PE/FE/NE/ORE(错误标志位)
        /*串口接收中断函数*/
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&Serial_RxData, 1);
    }
}