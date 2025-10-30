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

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        5

static rt_thread_t thread_pri_low = RT_NULL;
static rt_thread_t thread_pri_high = RT_NULL;

static void low_pri_entry(void *parameter);
static void high_pri_entry(void *parameter);

int main(void)
{
    thread_pri_low = rt_thread_create("low",
                            low_pri_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);
    if (thread_pri_low != RT_NULL)
        rt_thread_startup(thread_pri_low);
}

static void low_pri_entry(void *parameter)
{
    rt_kprintf("\rLow-priority thread run \r\n");
    thread_pri_high = rt_thread_create("high",
                            high_pri_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (thread_pri_high != RT_NULL)
        rt_thread_startup(thread_pri_high);

    rt_kprintf("Low-priority threads continue to run \r\n");
    while (1)
    {
        rt_thread_mdelay(500);
    }
}

static void high_pri_entry(void *parameter)
{
    rt_kprintf("High-priority thread run \r\n");
    while (1)
    {
        rt_thread_mdelay(500);
    }
}


