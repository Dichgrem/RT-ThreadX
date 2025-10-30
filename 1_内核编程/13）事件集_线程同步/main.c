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

#define EVENT_FLAG3 (1 << 3)
#define EVENT_FLAG5 (1 << 5)

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

static void send_entry(void *parameter);
static void recv_entry(void *parameter);

static rt_event_t dynamic_event = RT_NULL;

int main(void)
{
    dynamic_event = rt_event_create("event", RT_IPC_FLAG_PRIO);

    tid2 = rt_thread_create("recv_entry",
                            recv_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);

    tid1 = rt_thread_create("send_entry",
                            send_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

}

static void send_entry(void *parameter)
{
    rt_event_send(dynamic_event, EVENT_FLAG3);
    rt_thread_mdelay(100);
    rt_event_send(dynamic_event, EVENT_FLAG5);
    rt_thread_mdelay(100);
    rt_event_send(dynamic_event, EVENT_FLAG3|EVENT_FLAG5);
    rt_thread_mdelay(100);
}

static void recv_entry(void *parameter)
{
    rt_uint32_t e;
    while (1)
    {
        if (rt_event_recv(dynamic_event, (EVENT_FLAG3 | EVENT_FLAG5),
                          RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                          RT_WAITING_FOREVER, &e) == RT_EOK)
        {
            rt_kprintf("thread1: OR recv event 0x%x\n", e);
        }
        rt_thread_mdelay(50);
    }
}
