# STM32 智能小车（HAL 库 + CMake 版）

基于 **STM32F103C8T6** 的四轮智能小车，采用 **STM32 HAL 库** 开发，
构建系统为 **CMake + Ninja + arm-none-eabi-gcc**，开发环境为 VS Code + ST-Link。

支持四种工作模式：**蓝牙遥控**、**红外循迹**、**超声波自动避障**、**停车**。

> 同一套硬件另有标准库（SPL）实现版本，本仓库是 HAL 库的重写移植版。

---

## 功能特性

| 模式 | 实现方式 | 切换指令 |
|---|---|---|
| **蓝牙遥控** | 手机蓝牙串口助手发送单字符指令 | `A` |
| **红外循迹** | 四路红外 + 加权重心法（类 P 控制） | `B` |
| **自动避障** | HC-SR04 测距 + SG90 舵机三向扫描 | `C` |
| **停车** | 全部电机停转 | `S` |

- **OLED 实时状态显示**：屏幕第 1 行显示当前模式，避障模式下第 3 行显示实时距离
- **蓝牙断连保护**：读取 HC-04/05 的 `STATE` 引脚，连接断开时立即停车，避免小车失控
- **软件 I2C 驱动 OLED**：不占用硬件 I2C，节省引脚

---

## 硬件清单

| 序号 | 器件 | 型号 / 规格 | 数量 |
|---|---|---|---|
| 1 | 主控 | STM32F103C8T6（LQFP48，64KB Flash / 20KB RAM） | 1 |
| 2 | 电机驱动 | TB6612FNG（双路 H 桥，STBY 直接接 VCC 常使能） | 1 |
| 3 | 直流电机 | TT 减速电机（配车轮） | 4 |
| 4 | 舵机 | SG90（180°） | 1 |
| 5 | 超声波 | HC-SR04 | 1 |
| 6 | 循迹传感器 | 四路红外（LM324 比较器，数字量输出） | 1 |
| 7 | 显示屏 | SSD1306 OLED 0.96"（128×64，I2C） | 1 |
| 8 | 蓝牙模块 | HC-06（4 脚）或 HC-04/05（6 脚带 STATE） | 1 |
| 9 | 下载调试 | ST-Link V2 | 1 |
| 10 | 供电 | 4× 5 号电池盒（6V）+ AMS1117-3.3 | 1 套 |

> 接线细节见 [`Hardware.md`](Hardware.md)（含各模块具体接法）。

---

## 引脚分配

### 电机驱动（TB6612FNG）

| 引脚 | 功能 | 说明 |
|---|---|---|
| PA1 | PWMA | TIM2_CH2，20kHz PWM，左轮调速 |
| PA2 | PWMB | TIM2_CH3，20kHz PWM，右轮调速 |
| PA4 | AIN2 | 左轮方向 |
| PA5 | AIN1 | 左轮方向 |
| PA6 | BIN1 | 右轮方向 |
| PA7 | BIN2 | 右轮方向 |
| — | STBY | **直接接 VCC**，不占 MCU 引脚 |

### 其它外设

| 引脚 | 功能 | 归属模块 |
|---|---|---|
| PA8 | TIM1_CH1，50Hz PWM | SG90 舵机信号 |
| PA9 | USART1_TX | 蓝牙 RXD |
| PA10 | USART1_RX | 蓝牙 TXD |
| PB0 | GPIO 输入 | 红外 X4（DO4） |
| PB1 | GPIO 输入 | 红外 X3（DO3） |
| PB10 | GPIO 输入 | 红外 X2（DO2） |
| PB11 | GPIO 输入 | 红外 X1（DO1） |
| PB8 | GPIO 开漏输出 | OLED SCL（软件 I2C） |
| PB9 | GPIO 开漏输出 | OLED SDA（软件 I2C） |
| PB12 | GPIO 输出 | HC-SR04 Trig |
| PB13 | GPIO 输入 | HC-SR04 Echo |
| PB14 | GPIO 输入（下拉） | 蓝牙 STATE 连接状态 |
| PA13 / PA14 | SWDIO / SWCLK | ST-Link 下载调试 |

### 定时器分配

| 定时器 | PSC | ARR | 结果 | 用途 |
|---|---|---|---|---|
| TIM1 | 71 | 19999 | 50Hz（1 计数 = 1µs） | 舵机，CCR 直接等于脉宽 µs |
| TIM2 | 35 | 99 | 20kHz | 电机 PWM（Duty = CCR/100） |
| TIM4 | 71 | 59999 | 1 计数 = 1µs | 超声波回波计时 |

### 系统参数

- 系统时钟：HSE 8MHz × PLL9 = **72MHz**（APB1 = 36MHz，APB2 = 72MHz）
- 串口：USART1，**9600-8-N-1**（与 HC-06/04 默认波特率一致）

---

## 软件架构

```
.
├── Core/
│   ├── Inc/                        # 用户模块头文件
│   └── Src/                        # 用户模块源文件 + CubeMX 生成文件
├── Drivers/                        # ST 官方 HAL 库与 CMSIS（第三方）
├── cmake/
│   ├── gcc-arm-none-eabi.cmake     # 交叉编译工具链配置
│   └── stm32cubemx/                # CubeMX 生成的子工程（勿手改）
├── .vscode/                        # 编译任务 / 调试配置 / 代码提示
├── CMakeLists.txt                  # 顶层构建脚本
├── CMakePresets.json               # 构建预设（Debug / Release）
├── STM32F103xx_FLASH.ld            # 链接脚本
├── STM32_Hal_Car_Projects.ioc      # CubeMX 工程文件
└── Hardware.md                     # 接线说明
```

### 模块划分

| 模块 | 文件 | 职责 |
|---|---|---|
| **PWM** | `PWM.c/.h` | TIM2 双通道 PWM 初始化与占空比设置 |
| **Motor** | `Motor.c/.h` | 方向脚 + 速度组合，处理左右轮极性 |
| **Car** | `Car.c/.h` | 整车动作封装：`Car_Forward` / `Car_Back` / `Car_TurnLeft` / `Car_TurnRight` / `Car_Stop` |
| **Servo** | `Servo.c/.h` | TIM1_CH1 输出 50Hz，`CCR = 500 + angle × 2000/180` |
| **HC_SR04** | `HC_SR04.c/.h` | 超声波测距、舵机三向扫描、`Auto_Run` 避障决策 |
| **IR_Line_Track** | `IR_Line_Track.c/.h` | 四路红外加权重心法循迹 |
| **BlueToothCtrl** | `BlueToothCtrl.c/.h` | 蓝牙指令解析 + STATE 断连保护 |
| **Serial** | `Serial.c/.h` | USART1 中断收发，回调里重挂接收并处理 ORE |
| **OLED** | `OLED.c/.h` + `OLED_Front.h` | SSD1306 软件 I2C 驱动与 8×16 字库 |
| **Delay** | `Delay.c/.h` | 基于 DWT 周期计数器的 µs 级精确延时 |

---

## 编译

### 前置依赖

- `cmake` ≥ 3.22
- `ninja`
- `arm-none-eabi-gcc`（需在 **PATH** 中，工具链文件靠名字查找）

### 命令行

```bash
# 首次配置
cmake --preset Debug

# 日常增量编译
cmake --build --preset Debug
```

### VS Code

| 操作 | 效果 |
|---|---|
| `Ctrl+Shift+B` | **从零重建**（删 `build/Debug` → 重新配置 → 全量编译） |
| 命令面板 → `Tasks: Run Task` → `STM32: Build (增量)` | 只编译改动的文件，日常用这个 |

> ⚠️ 工程目录**改名或移动位置后必须从零重建**。
> `build/Debug` 里的 `CMakeCache.txt`、`build.ninja`、`compile_commands.json`
> 存的全是写死的绝对路径，路径一变就全部失效。

### 产物

```
build/Debug/STM32_Hal_Car_Projects.elf
```

当前资源占用：**RAM 1896 B (9.26%) / FLASH 20492 B (31.27%)**。

---

## 烧录与调试

1. ST-Link 接 SWDIO / SWCLK / GND（**不向小车供电**，小车自带电池）
2. VS Code 中按 `F5` 启动调试（ST-Link GDB Server）
3. 调试配置 `runEntry` 为 `main`，因此会先停在 `HAL_Init()`，**再按一次 `F5` 才是全速运行**
4. 想让程序独立运行：`Shift+F5` 结束调试后**拔掉 ST-Link 的 USB**，否则芯片可能停在 halt 状态

---

## 使用说明

### 模式切换（蓝牙串口助手发送单字符）

| 字符 | 模式 |
|---|---|
| `A` | 蓝牙遥控 |
| `B` | 红外循迹 |
| `C` | 自动避障 |
| `S` | 停车 |

### 蓝牙遥控指令（仅在 `A` 模式下有效）

| 字符 | 动作 |
|---|---|
| `0` | 停止 |
| `1` | 前进 |
| `2` | 后退 |
| `3` | 左转 |
| `4` | 右转 |

> 遥控为「**点按走到底**」语义：发一次指令持续执行，直到收到新指令。
> 蓝牙连接断开时（`STATE` 引脚为低），`BTControl()` 会立即停车。

---

## 关键算法参数

### 红外循迹（加权重心法）

四个探头实测中心间距为 `X1 —2.8— X2 —0.7— X3 —2.8— X4`（cm），
据此得到相对车体几何中心的权重：

```c
static const float Weight[4] = {-3.15f, -0.35f, 0.35f, 3.15f};
```

| 参数 | 值 | 含义 |
|---|---|---|
| `IR_BASE_Speed` | 40.0 | 基础速度 |
| `Turn_Gain` | 25.0 | 偏差增益（相当于 P 控制里的 Kp） |
| `SEEK_Speed` | 50 | 丢线后的原地寻找速度 |
| `LOST_TICK` | 10 | 连续 10 个周期检测不到线 → 判定为终点，停车 |
| `BLACK_STOP` | 12 | 连续 12 个周期四路全黑 → 停车 |

控制律：

```
Error = Σ(Weight[i] × active[i]) / Count
Left  = IR_BASE_Speed + Error × Turn_Gain
Right = IR_BASE_Speed − Error × Turn_Gain
```

### 超声波避障

| 参数 | 值 | 含义 |
|---|---|---|
| `STOP_CM` | 10.0 | 小于该距离 → 判定障碍，转向 |
| `GO_CM` | 15.0 | 大于该距离 → 判定可通行（10~15 之间保持上一状态，形成迟滞） |
| `SCAN_LEFT` / `SCAN_FRONT` / `SCAN_RIGHT` | 150° / 90° / 30° | 三向扫描角度 |
| `SCAN_Time_ms` | 200 | 每个角度等待舵机转到位的时间 |
| `Ahead_SPEED` / `TURN_SPEED` | 50 / 60 | 前进速度 / 原地转向速度 |
| `TURN_TIME_MS` | 350 | 转向持续时间，约 45° |

---

## 已知限制

- **四路全黑直接停车**：无法区分"终点黑色方块"与"横向黑线/十字"，当前策略是连续 `BLACK_STOP` 个周期全黑即停车。这是为规避死胡同而做的取舍。
- **无路径规划**：面对分岔路口只会直行穿越，不做分支选择。
- **无闭环速度控制**：未接入编码器，电机速度为开环 PWM。
- **单片循迹**：权重按单个探头有/无黑线计算，若黑线宽于探头间距会出现多路同时触发，此时按穿越处理。
- **避障为单点决策**：只比较左右距离取较远一侧，不做多步前瞻。

---

## 第三方代码与许可

- `Drivers/` 目录下的 STM32 HAL 驱动与 CMSIS 为 **STMicroelectronics** 官方代码，
  遵循其 BSD-3-Clause 许可（见各文件头部版权声明），**并非本项目原创**。
- 本项目自行编写的代码位于 `Core/Src` 与 `Core/Inc`（除 CubeMX 生成的
  `main.c` 骨架、`gpio.c`、`tim.c`、`usart.c`、`stm32f1xx_it.c`、
  `stm32f1xx_hal_msp.c`、`sysmem.c`、`syscalls.c` 之外）。

---

## 相关文档

- [`Hardware.md`](Hardware.md) —— 各模块详细接线说明
- `STM32_Hal_Car_Projects.ioc` —— 用 STM32CubeMX 打开可查看/修改全部外设配置
