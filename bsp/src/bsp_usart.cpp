#include "bsp_usart.hpp"
#include "usart.h"
#define USART_RX_BUF_SIZE 128   

static uint8_t usart1_rx_buf[USART_RX_BUF_SIZE];  // 调试串口接收区                                      
static uint8_t usart6_rx_buf[USART_RX_BUF_SIZE];  // 裁判系统接收区

void USART_Init(void)
{
  USART1_Init();
  USART6_Init();
}

void USART6_Init()
{
  // TODO: 配置 USART6 的 DMA 接收、发送及空闲中断，并启动接收。
  HAL_UARTEx_ReceiveToIdle_DMA(&huart6, usart6_rx_buf, USART_RX_BUF_SIZE);
};

void USART1_Init()
{
  // TODO: 配置 USART1 的 DMA 接收、发送及空闲中断，并启动接收。
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, usart1_rx_buf, USART_RX_BUF_SIZE);                                 
};

void USART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, enum USART_Mode mode)
{
  // TODO: 检查参数，并按 mode 选择阻塞、DMA 或中断发送。
  if(huart==nullptr||pData==nullptr||Size==0)//参数检查
  {
    return;
  }
  switch(mode){                                                                                                        
  case USART_MODE_BLOCK:                                                                                   
      HAL_UART_Transmit(huart, pData, Size, HAL_MAX_DELAY);  // 阻塞：CPU 等发完                           
      break;                                                                                               
  case USART_MODE_DMA:                                                                                     
      HAL_UART_Transmit_DMA(huart, pData, Size);             // DMA 发，立即返回                           
      break;                                                                                               
  case USART_MODE_IT:                                                                                      
      HAL_UART_Transmit_IT(huart, pData, Size);              // 中断发，一字节一中断                       
      break;                                                                                               
  }
}

void USART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  // TODO: 选择接收方式并启动 HAL UART 接收。
  HAL_UARTEx_ReceiveToIdle_DMA(huart, pData, Size);
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if(huart==&huart1)
  {
    // 处理 USART1 的接收事件
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, usart1_rx_buf, USART_RX_BUF_SIZE);
  }
  else if(huart==&huart6)
  {
    // 处理 USART6 的接收事件
  HAL_UARTEx_ReceiveToIdle_DMA(&huart6, usart6_rx_buf, USART_RX_BUF_SIZE);
  }
}