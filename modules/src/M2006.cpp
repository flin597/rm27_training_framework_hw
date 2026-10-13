#include "M2006.hpp"

M2006::M2006()
{
    // TODO: 根据 M2006 手册设置减速比、允许电流和速度/位置环 PID 参数。
    // 基类初始化为松开模式、零电流；完成配置前不允许驱动电机。
    gearBox=GearBox_M2006;
    maxCurrent=10000;
    speedPid.Tuning(100.0f, 0.4f, 0.0f);   // kp, ki, kd                        
    speedPid.maxOut = 20000.0f;              // 输出上限（电流单位）              
                                                                                
    positionPid.Tuning(20.0f, 0.0f, 0.0f);                                        
    positionPid.maxOut = 20.0f;              // 输出上限（速度单位 rad/s）
}

void M2006::setOutput()
{
// TODO: 分别实现松开、速度和位置模式；位置模式需要串联位置环与速度环。
    switch(controlMode)
    {
    case RELAX_MODE:
        currentSet=0;
        break;
    case SPD_MODE:
        speedPid.ref=speedSet;
        speedPid.fdb=motorFeedback.speedFdb;
        speedPid.UpdateResult();
        currentSet=(int16_t)speedPid.result;//PID输出就是电流.result是float类型，currentSet是int16_t类型，所以需要强制转换
        break;
    case POS_MODE:
        //外环（位置环）
        positionPid.ref=positionSet;
        positionPid.fdb=motorFeedback.positionFdb;
        positionPid.UpdateResult();
        
        //内环（速度环）
        speedPid.ref=positionPid.result;
        speedPid.fdb=motorFeedback.speedFdb;
        speedPid.UpdateResult();
        currentSet=(int16_t)speedPid.result;//同30行
        break;
    }    // TODO: 将 PID 输出限制在 maxCurrent 范围内后写入 currentSet。
    if(currentSet>(int16_t)maxCurrent)  currentSet=(int16_t)maxCurrent;
    else if(currentSet<-(int16_t)maxCurrent) currentSet=-(int16_t)maxCurrent;
}

M2006::MotorStateTypedef M2006::AliveCheck()
{
    // TODO: 比较 AliveFlag 与 Pre_AliveFlag，更新在线状态并保存本次计数。
    if(AliveFlag==Pre_AliveFlag)
    {
        // 如果 AliveFlag 没有变化，说明电机离线
        MotorState=MOTOR_OFFLINE;
    }
    else
    {
        MotorState=MOTOR_ONLINE;
    }
    Pre_AliveFlag=AliveFlag;
    return MotorState;
}
