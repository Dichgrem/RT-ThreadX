/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-25     RT-Thread    first version
 */

#include <rtthread.h>
/* 包含PIN设备相关头文件 */
#include <rtdevice.h>
#include <board.h>

/* 获取LED1~3引脚编号 */
#define LED1 GET_PIN(A,0)
#define LED2 GET_PIN(A,1)
#define LED3 GET_PIN(A,4)

/* 3.1 定义事件 */
#define E_LED1 (1<<3)
#define E_LED2 (1<<5)

/* 2.1 定义信号量句柄 */
rt_sem_t sled12;

/* 3.2 定义事件集句柄 */
rt_event_t eled123;

/* 1.4 线程tled1入口函数 */
void tled1_entry(void *parameter)
{
    int i=0; //LED闪烁计数
    /* 设置LED1引脚为推挽输出模式 */
    rt_pin_mode(LED1, PIN_MODE_OUTPUT);
    while(1)
    {
        /* 引脚电平翻转 */
        rt_pin_write(LED1, 1-rt_pin_read(LED1));
        rt_thread_mdelay(500);
        if(++i==6 && sled12!=RT_NULL)
        {
            /* 2.3 释放信号量 */
            rt_sem_release(sled12);
            rt_kprintf("tled1 release sem!\n");
        }
        if(++i==10 && eled123!=RT_NULL)
        {
            /* 3.4 发送事件E_LED1 */
            rt_event_send(eled123, E_LED1);
            rt_kprintf("tled1 send event\n");
        }
    }
}

/* 1.5 线程tled2入口函数 */
void tled2_entry(void *parameter)
{
    int i=0; //LED闪烁计数
    /* 设置LED1引脚为推挽输出模式 */
    rt_pin_mode(LED2, PIN_MODE_OUTPUT);
    while(1)
    {
        if(sled12!=RT_NULL)
        {
            /* 2.4 获取信号量 */
            rt_sem_take(sled12, RT_WAITING_FOREVER);
            rt_sem_delete(sled12);//只获取一次，获取到后删除
            sled12 = RT_NULL;
        }
        /* 引脚电平翻转 */
        rt_pin_write(LED2, 1-rt_pin_read(LED2));
        rt_thread_mdelay(1000);
        if(++i==10 && eled123!=RT_NULL)
        {
            /* 3.4 发送事件E_LED2 */
            rt_event_send(eled123, E_LED2);
            rt_kprintf("tled2 send event\n");
        }
    }
}

/* 1.6 线程tled3入口函数 */
void tled3_entry(void *parameter)
{
    rt_uint32_t e;
    /* 设置LED1引脚为推挽输出模式 */
    rt_pin_mode(LED3, PIN_MODE_OUTPUT);
    if(eled123!=RT_NULL)
    {
        /* 3.5 接收事件 */
        rt_event_recv(eled123, E_LED1 | E_LED2, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &e);
        rt_kprintf("tled3 received event\n");
        rt_event_delete(eled123);
        eled123 = RT_NULL;
    }

    while(1)
    {
        /* 引脚电平翻转 */
        rt_pin_write(LED3, 1-rt_pin_read(LED3));
        rt_thread_mdelay(2000);
    }
}
/* 编译，下载观察现象 */

int main(void)
{
    /* 利用信号量实现tled1和tled2同步 */
    /* 2.2 创建信号量 */
    sled12 = rt_sem_create("sled12", 0, RT_IPC_FLAG_PRIO);

    /* 利用事件集实现tled1、tled2和tled3同步 */
    /* 3.3 创建事件集 */
    eled123 = rt_event_create("eled123", RT_IPC_FLAG_PRIO);

    /* 1.1 定义3个线程句柄： tled1, tled2, tled3 */
    rt_thread_t tled1, tled2, tled3;

    /* 1.2 创建3个线程：tled1, tled2, tled3 */
    tled1 = rt_thread_create("tled1", tled1_entry, RT_NULL, 1024, 10, 10);
    tled2 = rt_thread_create("tled2", tled2_entry, RT_NULL, 1024, 11, 10);
    tled3 = rt_thread_create("tled3", tled3_entry, RT_NULL, 1024, 12, 10);

    /* 1.3 启动3个线程：tled1, tled2, tled3 */
    rt_thread_startup(tled1);
    rt_thread_startup(tled2);
    rt_thread_startup(tled3);

    return RT_EOK;
}
