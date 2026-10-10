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

### ThreadX + 堆栈
- 启用 X-CUBE-AZRTOS-F4 ThreadX 核心，节拍 1000 Hz（1 ms）
- HAL 时基：SysTick → TIM6（避免与 ThreadX 抢 SysTick）
- 主栈 0x800、堆 0x800（中断多 + C++ 可能 new 对象）
