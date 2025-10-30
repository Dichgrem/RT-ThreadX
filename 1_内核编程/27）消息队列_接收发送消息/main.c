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

static rt_mq_t dynamic_mq = RT_NULL;

int main(void)
{

    dynamic_mq = rt_mq_create("dmq", 10, 10, RT_IPC_FLAG_PRIO);

    tid1 = rt_thread_create("mq_send",
                            send_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    tid2 = rt_thread_create("mq_recv",
                            recv_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);
}

static void send_entry(void *parameter)
{
    char buf = 'A';
    char buf1 = 'B';
    char buf2 = 'C';

    rt_mq_send(dynamic_mq, &buf, sizeof(buf));
    rt_mq_send(dynamic_mq, &buf1, sizeof(buf1));
    rt_mq_urgent(dynamic_mq, &buf2, sizeof(buf2));
}

static void recv_entry(void *parameter)
{
    char buf = 0;
    while (1)
    {
        rt_thread_delay(1000);
        if (rt_mq_recv(dynamic_mq, &buf, sizeof(buf), RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("thread1: recv msg from msg queue, the content:%c\n", buf);
        }
    }
}
