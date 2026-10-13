#ifndef THREADS_HPP
#define THREADS_HPP
#include "tx_api.h"

#ifdef __cplusplus
extern "C" {
#endif
void Threads_Init(void);
#ifdef __cplusplus
}
#endif

void imu_thread_entry(ULONG arg);
void control_thread_entry(ULONG arg);
void alive_thread_entry(ULONG arg);
extern TX_SEMAPHORE sem_alive_imu;
extern TX_SEMAPHORE sem_alive_control;
extern volatile bool g_imu_init_ok;
#endif