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

extern void mqtt_upload_entry(void *parameter);

int main(void)
{
    rt_thread_t t_mqtt_upload;
    t_mqtt_upload = rt_thread_create("tmqttupload", mqtt_upload_entry, RT_NULL, 2048, 20, 10);
    rt_thread_startup(t_mqtt_upload);
    return RT_EOK;
}
