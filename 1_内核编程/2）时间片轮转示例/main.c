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

#define THREAD_PRIORITY         10
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        10

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

static void entry1(void *parameter);
static void entry2(void *parameter);
static volatile int flag = 0;

int main(void)
{
    tid1 = rt_thread_create("entry1",
                            entry1, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    tid2 = rt_thread_create("entry2",
                            entry2, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);

    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
}

static volatile rt_tick_t tick_cnt_entry1 = 0;
static volatile rt_tick_t tick_cnt_entry2 = 0;
static volatile int tick_cnt_diff = 0;

static void entry1(void *parameter)
{
    while (1)
    {
        if (flag == 0)
        {
            tick_cnt_entry1 = rt_tick_get();
            tick_cnt_diff = tick_cnt_entry1 - tick_cnt_entry2;

            rt_kprintf("Thread1_cnt - Thread2_cnt = %d\r\n", tick_cnt_diff);
            flag = 1;
        }
    }
}

static void entry2(void *parameter)
{
    while (1)
    {
        if (flag == 1)
        {
            tick_cnt_entry2 = rt_tick_get();
            flag = 0;
        }
    }
}


