#include "threads.hpp"
//定义imu和control线程的信号量
TX_SEMAPHORE sem_alive_imu;
TX_SEMAPHORE sem_alive_control;
volatile bool g_imu_init_ok=false;//imu初始化标志位，false表示未初始化，true表示初始化完成;volatile意思是，这个值可能被别的线程改动，每次用都重新读内存，不要用缓存（imu写，alive读）

//定义句柄
static TX_THREAD imu_thread;
static TX_THREAD control_thread;
static TX_THREAD alive_thread;

//定义三块栈，作为每个线程的独立仓库
static ULONG imu_stack[256];//ULONG是threadX/RTOS常用的长整型类型
static ULONG control_stack[256];
static ULONG alive_stack[256];

void Threads_Init()
{
    // 创建信号量
    tx_semaphore_create(&sem_alive_imu, "alive_imu",0);
    tx_semaphore_create(&sem_alive_control, "alive_control",0);
    //创建imu,control,alive线程
    //参数含义:句柄，名字，线程要跑的函数，传给线程函数的参数，用不上就写0，栈内存起始地址（线程独立仓库），栈大小，7，优先级（数字越小优先级越高），8.抢占阈值，先和优先级填一样的，9.时间片（同一优先级不论六，写SLICE），10.是否自动启动
    tx_thread_create(&imu_thread, "imu_thread", imu_thread_entry, 0,
                     imu_stack, sizeof(imu_stack), 6
                     ,6, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&control_thread, "control_thread", control_thread_entry, 0,
                     control_stack, sizeof(control_stack), 7
                     ,7, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&alive_thread, "alive_thread", alive_thread_entry, 0,
                     alive_stack, sizeof(alive_stack), 5
                     ,5, TX_NO_TIME_SLICE, TX_AUTO_START);//alive线程优先级最高，保证能及时响应

}