/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-1      RV            first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define THREAD_PRIORITY         15
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_thread_t tid = RT_NULL;
static rt_thread_t tid1 = RT_NULL;

static rt_mutex_t dynamic_mutex1 = RT_NULL;
static rt_mutex_t dynamic_mutex2 = RT_NULL;

static void usr_entry(void *parameter);
static void usr1_entry(void *parameter);

int main(void)
{
    dynamic_mutex1 = rt_mutex_create("mtx1", RT_IPC_FLAG_PRIO);
    dynamic_mutex2 = rt_mutex_create("mtx2", RT_IPC_FLAG_PRIO);

    tid = rt_thread_create("usr",
                            usr_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    tid1 = rt_thread_create("usr1",
                            usr1_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
}


static void usr_entry(void *parameter)
{
    while (1)
    {
        rt_mutex_take(dynamic_mutex1, RT_WAITING_FOREVER);
        rt_kprintf("usr: locked dynamic_mutex1\n");
        rt_thread_delay(100);

        rt_kprintf("usr: try to lock dynamic_mutex2\n");
        rt_mutex_take(dynamic_mutex2, RT_WAITING_FOREVER);

        rt_kprintf("usr: locked dynamic_mutex2\n");

        rt_mutex_release(dynamic_mutex1);
        rt_mutex_release(dynamic_mutex2);
    }
}

static void usr1_entry(void *parameter)
{
    while (1)
    {
        rt_mutex_take(dynamic_mutex2, RT_WAITING_FOREVER);
        rt_kprintf("usr2: locked dynamic_mutex2\n");
        rt_thread_delay(100);

        rt_kprintf("usr2: try to lock dynamic_mutex1\n");
        rt_mutex_take(dynamic_mutex1, RT_WAITING_FOREVER);

        rt_kprintf("usr2: locked dynamic_mutex1\n");

        rt_mutex_release(dynamic_mutex1);
        rt_mutex_release(dynamic_mutex2);
    }
}

