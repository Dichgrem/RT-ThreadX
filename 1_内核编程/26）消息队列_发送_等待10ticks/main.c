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

static void send_entry(void *parameter);

static rt_mq_t dynamic_mq = RT_NULL;

int main(void)
{

    dynamic_mq = rt_mq_create("dmq", 10, 1, RT_IPC_FLAG_PRIO);

    tid1 = rt_thread_create("mq_send",
                            send_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
}

static void send_entry(void *parameter)
{
    rt_err_t error = 0;
    char buf = 'A';
    char buf1 = 'B';

    error = rt_mq_send_wait(dynamic_mq, &buf, sizeof(buf), 10);
    rt_kprintf("RUN IN send_entry info %s\r\n",error == 0? "RT_EOK":"Error");
    error = rt_mq_send_wait(dynamic_mq, &buf1, sizeof(buf1),10);
    rt_kprintf("RUN IN send_entry info %s\r\n",error == 0? "RT_EOK":"Error");
}
