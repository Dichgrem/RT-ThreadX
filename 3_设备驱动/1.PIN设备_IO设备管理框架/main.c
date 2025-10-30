/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-18      RV           first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

rt_device_t usr_pin; // 定义用户设备句柄

struct rt_device_pin_status pin_status = {RT_NULL, PIN_LOW};
struct rt_device_pin_mode pin_mode = {RT_NULL, PIN_MODE_OUTPUT}; // 设置GPIO输出模式

int main(void)
{
    unsigned int count = 1;
    rt_err_t error;

    rt_base_t pin_num = rt_pin_get("PF.11"); // 获取RT-Thread GPIO管脚号
    pin_status.pin = pin_num; // 初始化GPIO 管脚
    pin_mode.pin = pin_num;   // 初始化GPIO 管脚

    usr_pin = rt_device_find("pin"); //获取PIN设备管理句柄

    error = rt_device_open(usr_pin, RT_DEVICE_OFLAG_RDWR); //打开PIN设备
    if (error != RT_EOK)
    {
        rt_kprintf("PIN device open fail!");
    }

    error = rt_device_control(usr_pin, RT_NULL, &pin_mode); //设置PIN设备工作模式
    if (error != RT_EOK)
    {
        rt_kprintf("PIN device control fail!");
    }

    while (1)
    {
        pin_status.status = PIN_LOW;
        rt_device_write(usr_pin, RT_NULL, &pin_status, sizeof(pin_status)); //设置PIN低电平
        LOG_D("led on, count: %d", count);
        rt_thread_mdelay(500);

        pin_status.status = PIN_HIGH;
        rt_device_write(usr_pin, RT_NULL, &pin_status, sizeof(pin_status)); //设置PIN高电平
        LOG_D("led off");
        rt_thread_mdelay(500);
    }
}

