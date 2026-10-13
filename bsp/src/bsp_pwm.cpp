//
// Created by cosmosmount on 2025/9/2.
//

#include "bsp_pwm.hpp"

void PWM_Init(void)
{
     // TODO: 根据实际使用的定时器和通道完成 PWM 初始化。
    HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);
}

void PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    // TODO: 调用 HAL 启动指定定时器通道的 PWM，并检查返回状态。
    if(HAL_TIM_PWM_Start(htim, Channel)!=HAL_OK)
    {
        Error_Handler();
    }
}

void PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    // TODO: 调用 HAL 停止指定定时器通道的 PWM。
    HAL_TIM_PWM_Stop(htim,Channel);
}

void PWM_SetPeriod(TIM_HandleTypeDef *htim, float period_s)
{
    // TODO: 根据定时器时钟和预分频值，将秒转换为 ARR 并更新周期。
    uint32_t counts=(uint32_t)(HAL_RCC_GetPCLK2Freq()*2/(htim->Init.Prescaler+1)*period_s);//counts是一个周期要数的次数
    __HAL_TIM_SET_AUTORELOAD(htim,counts-1);//把ARR=counts-1写进寄存器
    __HAL_TIM_SET_COUNTER(htim,0);//计数值清零，从头数起

}

void PWM_SetDutyRatio(TIM_HandleTypeDef *htim, float dutyratio, uint32_t channel)
{
    // TODO: 将 [0, 1] 占空比换算为比较值，写入指定通道。
    uint32_t arr=__HAL_TIM_GET_AUTORELOAD(htim);//获取ARR的值
    __HAL_TIM_SET_COMPARE(htim,channel,(arr+1)*dutyratio);//把占空比换算为比较值，写进捕获/比较寄存器CCR
}
