#include "threads.hpp"
#include "LED.hpp"

static ULONG alive_count(TX_SEMAPHORE *sem)
{
    ULONG count=0;
    while(tx_semaphore_get(sem, TX_NO_WAIT)==TX_SUCCESS)
    {
        count++;
    }
    return count;
}

void alive_thread_entry(ULONG arg)                                
  {                                                                 
    (void)arg;                                                    
                                                                
    // 先睡 2 秒：给 imu 线程时间去初始化（Config/Calibrate里有一堆 sleep）                                                  
    tx_thread_sleep(2000);                                        
                                                                
    while (1)                                                     
    {
        LED::OffAll();           
        // 1. 看"初始化成功"的牌子                                
        if (!g_imu_init_ok) {                                      
            LED::On(LED::RED);   // 初始化失败，红灯亮
        }                    
        // 2. 清点 imu 线程的打卡，0 次 = 它卡死了                
        // TODO: 调你写的"数打卡"，传入 &sem_alive_imu，若 ==0就点个灯
        else if(alive_count(&sem_alive_imu)==0)
        {
           LED::Toggle(LED::RED); 
        }
        // 3. 清点 control 线程的打卡                             
        // TODO: 同上，传 &sem_alive_control                      
        else if(alive_count(&sem_alive_control)==0)
        {
           LED::Toggle(LED::RED); 
        }
        else
        {
            LED::On(LED::GREEN); // 两个线程都活着，绿灯亮
        }
        // 4. 每隔 500ms 检查一次                                 
        tx_thread_sleep(500);                                     
    }                                                             
}