/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-01     RV           first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define THREAD_PRIORITY         11
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        10

static rt_thread_t tid2 = RT_NULL;

static void recv_entry(void *parameter);

static rt_sem_t dynamic_sem = RT_NULL;

int main(void)
{
    dynamic_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_PRIO);

    tid2 = rt_thread_create("sem_recv",
                            recv_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);
}

static void recv_entry(void *parameter)
{
    while (1)
    {
        rt_kprintf("T/%d\r\n",rt_tick_get());
        rt_sem_trytake(dynamic_sem);
    }
}


