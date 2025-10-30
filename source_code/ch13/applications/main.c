/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       HYT
 * 2022-06-01     RT-Thread    first version
 *
 * 1. 创建3个线程：温湿度获取线程、温湿度显示线程和温湿度上传线程
 * 2. 创建两个消息队列，温湿度获取显示消息队列和温湿度获取上传消息队列
 * 3. aht10和OLED采用自动初始化机制初始化
 */

#include <rtthread.h>
#include "sensor_asair_aht10.h"
#include <rtdevice.h>
#include <sensor.h>
#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
#include <onenet.h>

/* aht10所用I2C总线 */
#define AHT10_I2C_BUS  "i2c4"

/* 定义温湿度数据结构 */
struct data_th{
    float temp;
    float humi;
};

char flag_online[] = "";

/* 3.1 定义消息队列句柄 */
rt_mq_t q_get_show, q_get_upload;


/* 1.4 线程入口函数 */
void t_data_get_entry(void *parameter)
{
    struct data_th data;//发送数据结构
    struct rt_sensor_data data_temp, data_humi;//读取数据结构

    /* 2.1 定义设备句柄 */
    rt_device_t dev_temp, dev_humi;

    /* 2.2 查找设备 */
    dev_temp = rt_device_find("temp_aht10");
    dev_humi = rt_device_find("humi_aht10");

    /* 2.3 打开设备 */
    rt_device_open(dev_temp, RT_DEVICE_FLAG_RDWR);
    rt_device_open(dev_humi, RT_DEVICE_FLAG_RDWR);
    while(1)
    {
        /* 2.4 读设备 */
        rt_device_read(dev_temp, 0, &data_temp, 1);
        rt_device_read(dev_humi, 0, &data_humi, 1);

        data.temp = data_temp.data.temp;
        data.humi = data_humi.data.humi;

        /* 3.3 发送消息 */
        rt_mq_send(q_get_show, &data, sizeof(data));
        rt_mq_send(q_get_upload, &data, sizeof(data));

        rt_thread_mdelay(1000);
    }
}

void t_data_show_entry(void *parameter)
{
    struct data_th data; //接收数据
    char temp[4]; //显示温度数据
    char humi[4]; //显示湿度数据

    while(1)
    {
        /* 3.4 接收消息 */
        rt_mq_recv(q_get_show, &data, sizeof(data), RT_WAITING_FOREVER);

        /* 整理数据并显示 */
        sprintf(temp,"%.1f",data.temp/10.0);
        ssd1306_SetCursor(82, 22);
        ssd1306_WriteString(temp, Font_7x10, White);
        sprintf(humi,"%.1f",data.humi/10.0);
        ssd1306_SetCursor(82, 37);
        ssd1306_WriteString(humi, Font_7x10, White);
        ssd1306_SetCursor(82, 52);
        ssd1306_WriteString(flag_online, Font_7x10, White);
        ssd1306_UpdateScreen();
    }
}

void t_data_upload_entry(void *parameter)
{
    struct data_th data;  //接收数据结构

    /* 4.1 Onenet初始化 */
    while(onenet_mqtt_init() != 0)
    {
        rt_thread_mdelay(100);
    }
    while(1)
    {
        /* 3.4 接收消息 */
        rt_mq_recv(q_get_upload, &data, sizeof(data), RT_WAITING_FOREVER);

        /* 4.2 上传温度 */
        if(onenet_mqtt_upload_digit("Temp", data.temp/10.0) == RT_EOK)
        {
            /* 上传成功，联网状态为ON */
            //rt_kprintf("upload ok!\n");
            strcpy(flag_online,"");
            strcpy(flag_online,"ON ");
        }
        else
        {
            /* 上传失败，联网状态为OFF */
            rt_kprintf("upload failed!\n");
            strcpy(flag_online,"");
            strcpy(flag_online,"OFF");
        }

        rt_thread_mdelay(10);

        /* 4.2 上传湿度 */
        if(onenet_mqtt_upload_digit("Humi", data.humi/10.0) == RT_EOK)
        {
            strcpy(flag_online,"");
            strcpy(flag_online,"ON ");
        }
        else
        {
            strcpy(flag_online,"");
            strcpy(flag_online,"OFF");
        }
    }
}

int main(void)
{
    /* 1.1 定义线程句柄 */
    rt_thread_t t_data_get, t_data_show, t_data_upload;

    /* 3.2 创建消息队列 */
    q_get_show = rt_mq_create("qgetshow", sizeof(struct data_th), 10, RT_IPC_FLAG_FIFO);
    q_get_upload = rt_mq_create("qgetupload", sizeof(struct data_th), 10, RT_IPC_FLAG_FIFO);

    /* 1.2 创建线程 */
    t_data_get = rt_thread_create("tdataget", t_data_get_entry, RT_NULL, 1024, 15, 10);
    t_data_show = rt_thread_create("tdatashow", t_data_show_entry, RT_NULL, 1024, 16, 10);
    t_data_upload = rt_thread_create("tdataupload", t_data_upload_entry, RT_NULL, 2048, 17, 10);

    /* 1.3 启动线程 */
    rt_thread_startup(t_data_get);
    rt_thread_startup(t_data_show);
    rt_thread_startup(t_data_upload);

    return RT_EOK;
}

/* aht10自动初始化 */
int rt_hw_aht10_port(void)
{
    struct rt_sensor_config cfg;
    cfg.intf.dev_name  = AHT10_I2C_BUS;
    cfg.intf.user_data = (void *)AHT10_I2C_ADDR;
    rt_hw_aht10_init("aht10", &cfg);
    return RT_EOK;
}
INIT_ENV_EXPORT(rt_hw_aht10_port);

/* OLED自动初始化 */
int oled_init(void)
{
    ssd1306_Init();//OLED初始化
    ssd1306_Fill(Black);//黑底白字
    ssd1306_SetCursor(10, 0);//显示位置横向，纵向
    ssd1306_WriteString("Smart Home", Font_11x18, White);//显示字符串
    ssd1306_SetCursor(2, 22);
    ssd1306_WriteString("TEMP:", Font_7x10, White);
    ssd1306_SetCursor(2, 37);
    ssd1306_WriteString("HUMI:", Font_7x10, White);
    ssd1306_SetCursor(2, 52);
    ssd1306_WriteString("Onenet:", Font_7x10, White);
    ssd1306_UpdateScreen();//更新显示
    return RT_EOK;
}
INIT_DEVICE_EXPORT(oled_init);

