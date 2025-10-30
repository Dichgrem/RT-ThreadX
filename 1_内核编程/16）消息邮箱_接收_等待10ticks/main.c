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

static rt_thread_t tid2 = RT_NULL;

static void recv_entry(void *parameter);

static rt_mailbox_t dynamic_mb = RT_NULL;

int main(void)
{
    dynamic_mb = rt_mb_create("dmb", 10, RT_IPC_FLAG_PRIO);

    tid2 = rt_thread_create("mb_recv",
                            recv_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);
}

static void recv_entry(void *parameter)
{
    char *str;
    while (1)
    {
        rt_kprintf("T %d\r\n",rt_tick_get());
        rt_mb_recv(dynamic_mb, (rt_ubase_t *)&str, 10);
    }
}

