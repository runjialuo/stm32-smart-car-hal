#include "Delay.h"
#include "main.h"        // 里面已经 #include "stm32f1xx_hal.h"
#include "OLED.h"        // 本模块自己的头文件
#include "OLED_Front.h"   // 字模库（⚠️ 只能在这一个 .c 里包含，见文末说明）
#include "stdint.h"

/*====================== 引脚操作宏 ======================
 * 引脚配置在 CubeMX 里已经完成（见 Core/Src/gpio.c）：
 *   PB8 = OLED_SCL，开漏输出 + 内部上拉，初值高
 *   PB9 = OLED_SDA，开漏输出 + 内部上拉，初值高
 * 所以本文件【不需要】再写任何 GPIO 初始化，只需要写电平。
 *
 * 为什么必须是"开漏 + 上拉"？
 *   I2C 总线的电气规则是"线与"——任何设备只能把线拉低，不能主动拉高。
 *   开漏输出正好符合：输出 0 = 把线拉低；输出 1 = 引脚变高阻，
 *   靠上拉电阻把线拉回高电平。
 *   如果配成推挽输出，一方拉高一方拉低，就直接对顶短路了。
 *======================================================*/
static void OLED_W_SCL(uint8_t x){
    HAL_GPIO_WritePin(OLED_SCL_GPIO_Port, OLED_SCL_Pin, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    Delay_us(2);
}
static void OLED_W_SDA(uint8_t x){
    HAL_GPIO_WritePin(OLED_SDA_GPIO_Port, OLED_SDA_Pin, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    Delay_us(2);
}


/**
  * @brief  I2C 起始条件：SCL 为高电平期间，SDA 由高变低
  */
void OLED_I2C_Start(void)
{
    OLED_W_SDA(1);
    OLED_W_SCL(1);
    OLED_W_SDA(0);      // SCL 还是高，SDA 拉低 → 起始条件成立
    OLED_W_SCL(0);      // 钳住总线，准备发数据
}

/**
  * @brief  I2C 停止条件：SCL 为高电平期间，SDA 由低变高
  */
void OLED_I2C_Stop(void)
{
    OLED_W_SDA(0);
    OLED_W_SCL(1);
    OLED_W_SDA(1);      // SCL 高时 SDA 拉高 → 停止条件成立
}

/**
  * @brief  I2C 发送一个字节（高位先发）
  * @param  Byte 要发送的一个字节
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        // 0x80 >> i 依次取出 bit7..bit0；!! 把结果规范成 0 或 1
        OLED_W_SDA(!!(Byte & (0x80 >> i)));
        OLED_W_SCL(1);      // 上升沿，从机采样
        OLED_W_SCL(0);      // 拉低，准备下一位
    }
    OLED_W_SCL(1);          // 第 9 个时钟：留给应答位
    OLED_W_SCL(0);          // 本程序不读应答，直接放过
}

/**
  * @brief  OLED 写命令
  * @param  Command 要写入的命令
  */
void OLED_WriteCommand(uint8_t Command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);    // 从机地址（SSD1306 写模式）
    OLED_I2C_SendByte(0x00);    // 0x00 = 接下来是"命令"
    OLED_I2C_SendByte(Command);
    OLED_I2C_Stop();
}

/**
  * @brief  OLED 写数据
  * @param  Data 要写入的数据
  */
void OLED_WriteData(uint8_t Data)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);    // 从机地址
    OLED_I2C_SendByte(0x40);    // 0x40 = 接下来是"数据"
    OLED_I2C_SendByte(Data);
    OLED_I2C_Stop();
}

/**
  * @brief  OLED 设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7（页）
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127（列）
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                    // 设置页地址 Y
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));    // 设置列地址高 4 位
    OLED_WriteCommand(0x00 | (X & 0x0F));           // 设置列地址低 4 位
}

/**
  * @brief  OLED 清屏（把 8 页 × 128 列全写 0）
  */
void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for (i = 0; i < 128; i++)
        {
            OLED_WriteData(0x00);
        }
    }
}

/**
  * @brief  OLED 显示一个字符（8×16 点阵）
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的 ASCII 可见字符
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;
    // 8×16 的字符占 2 页，上半页
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i]);
    }
    // 下半页
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);
    }
}

/**
  * @brief  OLED 显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

/**
  * @brief  求 X 的 Y 次方（给显示数字用）
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
  * @brief  OLED 显示十进制无符号数字
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED 显示十进制带符号数字（会多占 1 位放 +/-）
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0)
    {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
  * @brief  OLED 显示十六进制数字
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++)
    {
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10)
        {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        }
        else
        {
            OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}

/**
  * @brief  OLED 显示二进制数字
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
    }
}

/**
  * @brief  OLED 初始化
  * @note   引脚配置已由 MX_GPIO_Init() 完成，这里只做"上电延时 + 发初始化命令"
  */
void OLED_Init(void)
{
    HAL_Delay(100);         // 上电等电源稳定（SPL 版是双重 for 死循环，HAL 直接用它自带的毫秒延时）

    // 确保总线处于空闲态（SCL/SDA 都为高）
    OLED_W_SCL(1);
    OLED_W_SDA(1);

    OLED_WriteCommand(0xAE);    // 关闭显示

    OLED_WriteCommand(0xD5);    // 设置显示时钟分频比 / 振荡器频率
    OLED_WriteCommand(0x80);

    OLED_WriteCommand(0xA8);    // 设置多路复用率
    OLED_WriteCommand(0x3F);

    OLED_WriteCommand(0xD3);    // 设置显示偏移
    OLED_WriteCommand(0x00);

    OLED_WriteCommand(0x40);    // 设置显示开始行

    OLED_WriteCommand(0xA1);    // 左右方向：0xA1 正常 / 0xA0 左右反置

    OLED_WriteCommand(0xC8);    // 上下方向：0xC8 正常 / 0xC0 上下反置

    OLED_WriteCommand(0xDA);    // 设置 COM 引脚硬件配置
    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0x81);    // 设置对比度
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9);    // 设置预充电周期
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);    // 设置 VCOMH 取消选择级别
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4);    // 整个显示打开 / 关闭

    OLED_WriteCommand(0xA6);    // 正常 / 倒转显示

    OLED_WriteCommand(0x8D);    // 电荷泵
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF);    // 开启显示

    OLED_Clear();               // 清屏
}
