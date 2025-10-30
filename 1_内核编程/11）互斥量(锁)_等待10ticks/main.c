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

#define THREAD_PRIORITY         15
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

static void send_entry(void *parameter);
static void recv_entry(void *parameter);

static rt_mutex_t dynamic_mutex = RT_NULL;

int main(void)
{
    dynamic_mutex = rt_mutex_create("dmutex", RT_IPC_FLAG_PRIO);

    tid2 = rt_thread_create("mtx_recv",
                            recv_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);

    tid1 = rt_thread_create("mtx_send",
                            send_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

}

static void send_entry(void *parameter)
{
    rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);
    while (1)
    {
        rt_thread_delay(5);
    }
}

static void recv_entry(void *parameter)
{
    while (1)
    {
        rt_kprintf("T %d\r\n",rt_tick_get());
        rt_mutex_take(dynamic_mutex, 10);
    }
}

