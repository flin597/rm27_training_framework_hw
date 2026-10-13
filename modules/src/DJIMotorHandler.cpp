#include "DJIMotorHandler.hpp"
#include "bsp_can.hpp"

void DJIMotorHandler::registerMotor(DJIMotor *motor, CAN_HandleTypeDef *hcan, uint16_t canId)
{
    // TODO: 校验 CAN 句柄和 0x201~0x208 ID，再登记到对应总线与槽位。
    if(hcan!=&hcan1 &&hcan!=&hcan2)
    {
        return;
    }
    if(canId<0x201||canId>0x208)
    {
        return;
    }
    if(motor==nullptr)
    {
        return;
    }
    int slot=canId-0x201;
    int bus=(hcan==&hcan1)?0:1;
    DJIMotorList[bus][slot]=motor;
    motor->hcan=hcan;
    motor->canId=canId;
    // TODO: 按 ID 设置对应的 0x200/0x1FF 报文存在标志。->这类帧有没有内容要发布的布尔标志
    if(slot<4)
    {
        if(bus==0)
        {
            CAN1_0x200_Exist=true;
        }
        else
        {
            CAN2_0x200_Exist=true;
        }
    }
    else
    {
        if(bus==0)
        {
            CAN1_0x1FF_Exist=true;
        }
        else
        {
            CAN2_0x1FF_Exist=true;
        }
    }

}

void DJIMotorHandler::sendControlData()
{
    // TODO: 将各电机 currentSet 按大端序放入正确的 8 字节控制帧。
    //CAN1的0x200帧
    for(int i=0;i<4;i++)
    {
        DJIMotor *m=DJIMotorList[0][i];
        if(m!=nullptr)
        {
            uint8_t hi=(uint8_t)(m->currentSet>>8&0xFF);
            uint8_t lo=(uint8_t)(m->currentSet&0xFF);

            can1_send_data_0[2*i]=hi;
            can1_send_data_0[2*i+1]=lo;
        }
        else
        {
            can1_send_data_0[2*i]=0;
            can1_send_data_0[2*i+1]=0;
        }
    }
    //CAN1的0x1FF帧
    for(int i=0;i<4;i++)
    {
        DJIMotor *m=DJIMotorList[0][i+4];
        if(m!=nullptr)
        {
            uint8_t hi=(uint8_t)(m->currentSet>>8&0xFF);
            uint8_t lo=(uint8_t)(m->currentSet&0xFF);
            can1_send_data_1[2*i]=hi;
            can1_send_data_1[2*i+1]=lo;
        }
        else
        {
            can1_send_data_1[2*i]=0;
            can1_send_data_1[2*i+1]=0;
        }
    }

    //CAN2的0x200帧
    for(int i=0;i<4;i++)
    {
        DJIMotor *m=DJIMotorList[1][i];
        if(m!=nullptr)
        {
            uint8_t hi=(uint8_t)(m->currentSet>>8&0xFF);
            uint8_t lo=(uint8_t)(m->currentSet&0xFF);
            can2_send_data_0[2*i]=hi;
            can2_send_data_0[2*i+1]=lo;
        }
        else
        {
            can2_send_data_0[2*i]=0;
            can2_send_data_0[2*i+1]=0;
        }
    }
    //CAN2的0x1FF帧
    for(int i=0;i<4;i++)
    {
        DJIMotor *m=DJIMotorList[1][i+4];
        if(m!=nullptr)
        {
            uint8_t hi=(uint8_t)(m->currentSet>>8&0xFF);
            uint8_t lo=(uint8_t)(m->currentSet&0xFF);
            can2_send_data_1[2*i]=hi;
            can2_send_data_1[2*i+1]=lo;
        }
        else
        {
            can2_send_data_1[2*i]=0;
            can2_send_data_1[2*i+1]=0;
        }
    }

    
    // TODO: 仅发送包含已注册电机的报文；未完成前不发送 CAN 电流命令。
    if (CAN1_0x200_Exist)
    {
        CAN_Transmit(&hcan1, 0x200, can1_send_data_0, 8);
    }
    if (CAN1_0x1FF_Exist)
    {
        CAN_Transmit(&hcan1, 0x1FF, can1_send_data_1, 8);
    }
    if (CAN2_0x200_Exist)
    {
        CAN_Transmit(&hcan2, 0x200, can2_send_data_0, 8);
    }
    if (CAN2_0x1FF_Exist)
    {
        CAN_Transmit(&hcan2, 0x1FF, can2_send_data_1, 8);
    }
}

void DJIMotorHandler::updateFeedback(CAN_HandleTypeDef *hcan, uint8_t *rx_data, int index)
{
    // TODO: 校验总线、反馈数据和索引，再找到对应电机并调用 UpdateSensorData。
    if(hcan!=&hcan1&&hcan!=&hcan2)
    {
        return;
    }
    if(rx_data==nullptr)
    {
        return;
    }
    if(index<0||index>7)
    {
        return;
    }
    int bus=(hcan==&hcan1)?0:1;
    DJIMotor *motor=DJIMotorList[bus][index];
    if(motor!=nullptr)
    {
        UpdateSensorData(motor, rx_data);
    }
}

void DJIMotorHandler::UpdateSensorData(DJIMotor *motor, uint8_t *can_data)
{
    if(motor==nullptr||can_data==nullptr)
    {
        return;
    }
    // TODO: 从 CAN 数据解析编码器、转速、电流和温度。
    uint16_t ecd  = (can_data[0] << 8) | can_data[1];            // 编码器角度，uint16 大端
    int16_t  rpm  = (int16_t)((can_data[2] << 8) | can_data[3]); // 转速，int16，大端   单位是rpm（转每分钟），转化成rad/s需要乘以2*pi/60         
    int16_t  cur  = (int16_t)((can_data[4] << 8) | can_data[5]); // 电流   int16_t 大端       
    uint8_t  temp = can_data[6];                                 // 温度 uint8_t
    motor->motorFeedback.currentFdb = cur; // 电流反馈，原始电流值
    motor->motorFeedback.temperatureFdb = temp;  // 温度反馈，单位℃
    // TODO: 处理编码器回绕，并按减速比换算输出轴位置和角速度。
    float ratio=(motor->gearBox==GearBox_M2006)?36.0f:1.0f;
    motor->motorFeedback.speedFdb=rpm*(2.0f*3.14159265f/60.0f)/ratio;//转子转36圈输出轴转一圈
    
    int16_t diff=(int16_t)(ecd-motor->motorFeedback.last_ecd);
    if(diff<-4096)
    {
        diff+=8192;
    }
    else if(diff>4096)
    {
        diff-=8192;
    }
    motor->motorFeedback.last_ecd=ecd;
    float delta =diff/8192.0f*2.0f*3.14159265f/ratio;//输出轴的角度增量
    motor->motorFeedback.positionFdb+=delta;
    motor->AliveFlag++;
}

void DJIMotorHandler::AllMotorAliveCheck()
{
    // TODO: 遍历已注册电机并调用 AliveCheck。
    for(int bus=0;bus<2;bus++)
    {
        for(int slot=0;slot<8;slot++)
        {
            DJIMotor *m=DJIMotorList[bus][slot];
            if(m!=nullptr)
            {
                m->AliveCheck();
            }
        }
    }
}
