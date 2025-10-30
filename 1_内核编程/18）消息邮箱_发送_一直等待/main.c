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

char buf = 'A';
char buf1 = 'B';
char buf2 = 'C';

static void send_entry(void *parameter);

static rt_mailbox_t dynamic_mb = RT_NULL;

int main(void)
{
    dynamic_mb = rt_mb_create("dmb", 1, RT_IPC_FLAG_PRIO);

    tid1 = rt_thread_create("mb_send",
                            send_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
}

static void send_entry(void *parameter)
{
    rt_mb_send_wait(dynamic_mb, (rt_ubase_t) &buf, RT_WAITING_FOREVER);
    rt_kprintf("RUN IN send_entry,1\r\n");
    rt_mb_send_wait(dynamic_mb, (rt_ubase_t) &buf1, RT_WAITING_FOREVER);
    rt_kprintf("RUN IN send_entry,2\r\n");
}

