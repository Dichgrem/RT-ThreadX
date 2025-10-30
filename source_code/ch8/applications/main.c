/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-04-19     RT-Thread    first version
 */

#include <rtthread.h>
#include <main.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* 4.编写线程入口函数 */
void thread_ledstatus_entry(void *parameter)
{
    /* LED1引脚初始化 */
    MX_GPIO_Init();
    /* LED1间隔0.5s闪烁3次 */
    for(int i=0; i<6; i++)
    {
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        rt_thread_mdelay(500);
    }

    /* LED1间隔1s闪烁 */
    while(1)
    {
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        rt_thread_mdelay(1000);
    }
}
int main(void)
{
    /* 1. 定义线程句柄 */
    rt_thread_t thread_ledstatus = RT_NULL;

    /* 2. 创建线程 */
    thread_ledstatus = rt_thread_create("ledstatus", thread_ledstatus_entry, RT_NULL, 1024, 10, 10);

    /* 3. 启动线程 */
    if (thread_ledstatus != RT_NULL) {
        rt_thread_startup(thread_ledstatus);
    }
    return RT_EOK;
}
