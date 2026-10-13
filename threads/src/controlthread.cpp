#include "threads.hpp"
#include "M2006.hpp"
#include "DJIMotorHandler.hpp"

static M2006 motor;

void control_thread_entry(ULONG arg)
{
    (void)arg;
    //注册到CAN1,反馈 ID 0x201
    DJIMotorHandler::Instance()->registerMotor(&motor,&hcan1,0x201);

    //设模式和目标
    motor.controlMode=DJIMotor::SPD_MODE;
    motor.speedSet=1.0f;

    //控制循环
    while(1)
    {
        motor.setOutput();
        DJIMotorHandler::Instance()->sendControlData();
        tx_thread_sleep(2);
        tx_semaphore_put(&sem_alive_control);
    }


}