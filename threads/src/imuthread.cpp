#include "threads.hpp"                                            
#include "BMI088.hpp"                                             
#include "bsp_pwm.hpp"                                            
                                                                
// BMI088 实例：它内部存着 acc_data、gyro_data、self_test 这些结果
static BMI088::cBMI088 imu;                                       
                                                                
void imu_thread_entry(ULONG arg)                                  
{                                                                 
    (void)arg;                                                    
                                                                
    // 1. 先启动 PWM（控温要靠 TIM10 加热）                       
    PWM_Init();                                                   
    // 2. 初始化 BMI088：配置 → 验 ID → 标定（顺序别乱，先把这 3个函数调一遍）
    // TODO: 你来填（配料表里找）
    imu.Config();
    imu.VerifyAccChipID();
    imu.VerifyGyroChipID();
    imu.Calibrate();
    // 3. 判断初始化成功没：加速度计和陀螺仪的 ID 都对了才算成功  
    //    提示：看 imu.self_test 里的 ACC_CHIP_ID_ERR 和       GYRO_CHIP_ID_ERR                                                  
    g_imu_init_ok =(imu.self_test.ACC_CHIP_ID_ERR==false&&imu.self_test.GYRO_CHIP_ID_ERR==false); /* TODO:两个错误标志都为 false 时才true */;
    if(!g_imu_init_ok)                                                  
    {
        while(1)
        {
            tx_thread_sleep(1000); // 初始化如果失败，卡死在这里
        }
    }
    // 4. 主循环：读数据 → 控温 → 报平安 → 睡                     
    while (1)                                                     
    {                                                             
        // TODO: 你来填（配料表里找，读加速度、读角速度、控温、put、sleep）

        imu.VerifyAccData();
        imu.VerifyGyroData();

        imu.ReadAccData(&imu.acc_data);
        // imu.ReadGyroData(&imu.gyro_data);
        
        // imu.TemperatureControl(50.0f);
        tx_semaphore_put(&sem_alive_imu);
        tx_thread_sleep(2);
    }                                                             
}