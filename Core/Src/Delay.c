/**************************************
使用Cortex-M3内核的周期计数器(DWT)实现微秒级延时
优点：1、独立于Systick的72MHz
      2、不用开时钟，也不配置寄存器
      3、节省定时器资源
***************************************/

#include "Delay.h"
#include "main.h"

/*
@延时函数初始化
CoreDebug:内核配置结构体
DEMCR:内核调试寄存器
TRCENA位：开启调试跟踪模块
DWT:数据观察点与追踪单位(内核调试组件，需打开开关才能工作)
CYCCNT:32位内核周期计数器,内核每走一个时钟周期CYCCNT+1(使用前先清零)
CYCCNTENA:CYCCNT计数使能位
*/
void Delay_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;     //打开内核跟踪单元
    DWT->CYCCNT = 0;                                    //周期计数清零        
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;                //启动CYCCNT计数
}

/*
@brief微秒级延时函数
SystemCoreClock：内核频率，stm32f103为(72MHz)
SystemCoreClock / 1000000U得到1us中会跑多少个时钟周期(1秒有1百万us)
U无符号整型
*/
void Delay_us(uint32_t xus)
{
    uint32_t start = DWT->CYCCNT;                    //记录延时开始时刻的计数值
    uint32_t tick = xus*(SystemCoreClock / 1000000U);//获取xus下进行的周期数
    while((DWT->CYCCNT - start) < tick);             //周期数等于tick时退出
}

/*@brief毫秒级延时函数*/
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}

/*
@brief秒延时函数(秒)
*/
void Delay_s(uint32_t xs)
{
    while(xs--)
    {
        Delay_us(1000000U);
    }
}
