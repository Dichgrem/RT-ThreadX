/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-31     RT-Thread    first version
 */


#include <rtthread.h>
#include <stdlib.h>

/* 数据类型结构体 */
struct data_ht{
    int id;
    int humi;
    int temp;
};

/* 2.1 定义消息队列句柄 */
rt_mq_t q_get_print;

/* 1.4 线程t_data_get1入口函数  */
void t_data_get1_entry(void *parameter)
{
    struct data_ht data; //定义数据
    while(1)
    {
        data.id = 1;
        data.humi = rand() % 100;
        data.temp = rand() % 100; //产生0~99的随机数
        /* 2.3 发送消息 */
        rt_mq_send(q_get_print, &data, sizeof(data));
        rt_thread_mdelay(1000);
    }
}

/* 1.4 线程t_data_get2入口函数  */
void t_data_get2_entry(void *parameter)
{
    struct data_ht data; //定义数据
    while(1)
    {
        data.id = 2;
        data.humi = rand() % 100;
        data.temp = rand() % 100; //产生0~99的随机数
        /* 2.3 发送消息 */
        rt_mq_send(q_get_print, &data, sizeof(data));
        rt_thread_mdelay(1000);
    }
}

void t_data_print_entry(void *parameter)
{
    struct data_ht data;
    while(1)
    {
        /* 2.4 接收消息 */
        rt_mq_recv(q_get_print, &data, sizeof(data), RT_WAITING_FOREVER);
        rt_kprintf("id:%d, humi:%d, temp:%d\n",data.id, data.humi, data.temp);
    }
}

int main(void)
{

    /* 1.1 定义线程句柄 */
    rt_thread_t t_data_get1, t_data_get2, t_data_print;

    /* 2.2 创建消息队列 */
    q_get_print = rt_mq_create("qgetprint", sizeof(struct data_ht), 10, RT_IPC_FLAG_FIFO);

    /* 1.2 创建线程 */
    t_data_get1 = rt_thread_create("tdataget1", t_data_get1_entry, RT_NULL, 1024, 10, 10);
    t_data_get2 = rt_thread_create("tdataget2", t_data_get2_entry, RT_NULL, 1024, 10, 10);
    t_data_print = rt_thread_create("tdataprint", t_data_print_entry, RT_NULL, 1024, 11, 10);

    /* 1.3 启动线程 */
    rt_thread_startup(t_data_get1);
    rt_thread_startup(t_data_get2);
    rt_thread_startup(t_data_print);

    return RT_EOK;
}
