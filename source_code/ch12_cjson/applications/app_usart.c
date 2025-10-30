/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-10     hytha       the first version
 */

/* 该文件完成不定长数据接收 */
#include "app_usart.h"

/* 接收中断回调函数 */
rt_err_t uart_recv_callback(rt_device_t dev, rt_size_t size)
{
    if(size>0)
    {
        rt_sem_release(s_recv);//释放信号量
    }
    return RT_EOK;
}

/* 获取一个字符 */
char uart_get_char(void)
{
    char ch;
    /* 读到字符则返回字符，否则挂起等待 */
    while (rt_device_read(serial, 0, &ch, 1) == 0)
    {
        rt_sem_control(s_recv, RT_IPC_CMD_RESET, RT_NULL);//信号量复位
        rt_sem_take(s_recv, RT_WAITING_FOREVER);//等待获取信号量
    }
    return ch;
}

/* 获取不定长字符串 */
void uart_get_string(char data[])
{
    char ch;
    static int i = 0;
    while (1)
    {
        ch = uart_get_char();//获取一个字符
        /* 判断结束标志 */
        if (ch == DATA_CMD_END)
        {
            data[i++] = '\0';
            i = 0;
            break;
        }

        /* 字符串长度不超过最大长度 */
        i = (i >= ONE_DATA_MAXLEN - 1) ? ONE_DATA_MAXLEN - 1 : i;
        data[i++] = ch;
    }
}

/* 串口自动初始化 */
int uart_init(void)
{
    s_recv = rt_sem_create("srecv", 0, RT_IPC_FLAG_FIFO);
    serial = rt_device_find("uart3");
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_recv_callback);
    return 0;
}
INIT_DEVICE_EXPORT(uart_init);
