/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-08     RT-Thread    first version
 */

#include <rtthread.h>
#include "app_cjson.h"
#include "app_usart.h"

msg_recv_def msg_recv;//接收数据
msg_upload_def msg_upload;//上传数据

void t_jsonpara_entry(void *parameter)
{
    char data[ONE_DATA_MAXLEN];
    while (1)
    {
        /* 串口接收JSON数据 */
        uart_get_string(data);
        /* 解析数据 */
        msg_recv_parse(&msg_recv, data);
        /* 更新上传数据 */
        msg_upload.fan = msg_recv.fan;
        rt_thread_mdelay(10);
    }
}

void t_msg_upload_entry(void *parameter)
{
    char *out = NULL;

    /* 获取数据 */
    rt_strncpy(msg_upload.location, "room1", 10);
    msg_upload.fan = RT_TRUE;
    msg_upload.temp = 25.5;
    msg_upload.humi = 45;

    while(1)
    {
        /* 数据组包 */
        out = msg_upload_pack(&msg_upload);
        /* 上传数据 */
        rt_device_write(serial, 0, out, rt_strlen(out));
        /* 释放内存 */
        free(out);
        rt_thread_mdelay(1000);

    }
}

int main(void)
{
    /* 创建并启动上传和解析线程 */
    rt_thread_t t_msg_upload, t_jsonpara;
    t_msg_upload = rt_thread_create("tmsgupload", t_msg_upload_entry, RT_NULL, 2048, 10, 10);
    rt_thread_startup(t_msg_upload);
    t_jsonpara = rt_thread_create("tjsonpara", t_jsonpara_entry, RT_NULL, 1024, 10, 10);
    rt_thread_startup(t_jsonpara);

    return RT_EOK;
}
