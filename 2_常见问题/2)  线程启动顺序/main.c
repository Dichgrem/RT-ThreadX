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

static void usr_entry(void *parameter);
static void usr1_entry(void *parameter);

int main(void)
{
    tid = rt_thread_create("usr",
                            usr_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    tid1 = rt_thread_create("usr1",
                            usr1_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 2, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
}

static void usr_entry(void *parameter)
{
    rt_kprintf("run in usr_entry \r\n");
    while (1)
    {
        rt_thread_delay(100);
    }
}

static void usr1_entry(void *parameter)
{
    rt_kprintf("run in usr1_entry \r\n");
    while (1)
    {
        rt_thread_delay(100);
    }
}

