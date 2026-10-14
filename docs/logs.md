# 调试 / 配置日志

## 10/10 CubeMX 板级外设配置

### 时钟
- 系统时钟 168 MHz：HSE 8 MHz → PLL（PLLM=4、PLLN=168、PLLP=2）
- APB1 = 42 MHz、APB2 = 84 MHz；定时器时钟 APB1Tim = 84 MHz、APB2Tim = 168 MHz

### GPIO（三色灯）
- PH10=蓝、PH11=绿、PH12=红，推挽输出，初始低电平（灭）
- 高电平点亮（N-MOS 低边驱动，YJL3400A）

### UART
- USART1（调试）：PA9=TX、PB7=RX
- USART6（裁判系统）：PG14=TX、PG9=RX
- 均配 DMA 收发 + 全局中断；接收用空闲中断(IDLE)处理变长帧
  - USART1：RX→DMA2_Stream2、TX→DMA2_Stream7
  - USART6：RX→DMA2_Stream1、TX→DMA2_Stream6
- 踩坑：模板默认 USART1_TX=PB6，但 PB6 实为 CAN2_TX，已改至 PA9

### CAN
- CAN1：PD0=RX、PD1=TX；CAN2：PB5=RX、PB6=TX
- 波特率 1 Mbps：PSC=7、BS1=3TQ、BS2=2TQ（42MHz ÷ (7×6) = 1 MHz）
- 均使能 RX0 中断（接收电机反馈）
- 踩坑：CubeMX 默认 CAN1 引脚为 PB8/PB9，但板子实际走线是 PD0/PD1，需手动改

### SPI1（BMI088）
- 重映射：PB3=SCK、PB4=MISO、PA7=MOSI；5.25 MHz；Mode 0
- 软件片选：PA4=加速度计 CS、PB0=陀螺仪 CS（初始高电平）
- 详见 spi.md

### I²C（IST8310）
- I2C3：SCL=PA8、SDA=PC9；快速模式 400 kHz；器件地址 0x0E（IST8310 选做）
- 详见 iic.md

### PWM（舵机）
- TIM8_CH3 = PI7，50 Hz（PSC=167、ARR=19999，脉宽 500~2500 = 0.5~2.5 ms）
- 暂无实体舵机，接线：信号→PI7、电源→5V、地→GND

### TIM10（BMI088 加热电阻，补漏）
- TIM10_CH1 = PF6，PWM（PSC=167、ARR=999 → 1 kHz），供 BMI088 的 TemperatureControl 加热用
- 坑：配置清单未列出，但框架 BMI088.hpp 引用了 htim10，编译时才暴露；CubeMX 默认引脚又是 PB8（错），板子实际是 PF6

### ThreadX + 堆栈
- 启用 X-CUBE-AZRTOS-F4 ThreadX 核心，节拍 1000 Hz（1 ms）
- HAL 时基：SysTick → TIM6（避免与 ThreadX 抢 SysTick）
- 主栈 0x800、堆 0x800（中断多 + C++ 可能 new 对象）

## 10/14 threads 线程阶段 + 上板

### threads 编译接线踩坑
- `board/CMakeLists.txt` 里 include 路径手滑写成 `../threads/includ`（少了 e），编译报「找不到 threads.hpp」；改回 `../threads/include`。
- imuthread.cpp 初始化成功判断最初写成 `!x==false`（C 里等于 `(!x)==false`，语义正好反了）。正确的是 `imu.self_test.ACC_CHIP_ID_ERR==false && imu.self_test.GYRO_CHIP_ID_ERR==false`。
- alive 线程点灯前没先 `LED::OffAll()`，多个颜色叠加成混色；改成每轮先 `OffAll()` 再按状态点灯。
- board/Drivers 出现 1434 个文件的换行符噪声（CubeMX 重新生成导致），用 `git restore board/Drivers` 清掉。

### CAN 滤波器修复（bsp_can.cpp，待提交）
- 原来 `CAN_FilterTypeDef` 没初始化，且 `SlaveStartFilterBank` 是给 CAN1 配完才赋值 → CAN1 拿到的是栈上垃圾值。
- 修复：`= {}` 清零整个结构体 + 把 `SlaveStartFilterBank=14` 提到 CAN1 配置之前（该字段是给主 CAN1 用的，告诉它从机 CAN2 的滤波器从第 14 组开始）。

### 首次上板（HardFault，待排查）
- 烧录成功，但一启动就 HardFault：`pc: 0x080082f0/f2`，停在 `tx_initialize_low_level.s` 的 `__tx_HardfaultHandler` 死循环，灯不亮。
- 排查方法：cortex-debug 里 `x/8wx $msp`，倒数第 2 个字就是崩溃 PC，据此定位崩溃行。
- 代码层面查过 BMI088/CAN/USART/ThreadX 底层未见明显 bug，怀疑板子问题（待确认）。
