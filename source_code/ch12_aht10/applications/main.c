/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-05     RT-Thread    first version
 */

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>
#include <sensor.h>

/* 1. 头文件包含及宏定义 */
#include "sensor_asair_aht10.h"
#define AHT10_I2C_BUS  "i2c4"

int main(void)
{
    /* 3. 定义传感器数据结构 */
    struct rt_sensor_data aht_temp, aht_humi;

    /* 4. 定义AHT10传感器温度和湿度句柄 */
    rt_device_t dev_aht10_temp, dev_aht10_humi;

    /* 5. 查找AHT10传感器 */
    dev_aht10_temp = rt_device_find("temp_aht10");
    dev_aht10_humi = rt_device_find("humi_aht10");

    /* 6. 打开AHT10传感器设备 */
    rt_device_open(dev_aht10_temp, RT_DEVICE_FLAG_RDONLY);
    rt_device_open(dev_aht10_humi, RT_DEVICE_FLAG_RDONLY);

    while (1)
    {
        /* 7. 分别读取温度和湿度，读取结果保存至对应数据结构 */
        rt_device_read(dev_aht10_temp, 0, &aht_temp, 1);
        rt_device_read(dev_aht10_humi, 0, &aht_humi, 1);

        /* 8. 打印温湿度值 */
        rt_kprintf("temp: %d.%d\n", aht_temp.data.temp/10, aht_temp.data.temp%10);
        rt_kprintf("humi: %d\n", aht_humi.data.humi/10);

        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

/* 2. AHT10初始化 */
int rt_hw_aht10_port(void)
{
    struct rt_sensor_config cfg;
    cfg.intf.dev_name  = AHT10_I2C_BUS;
    cfg.intf.user_data = (void *)AHT10_I2C_ADDR;
    rt_hw_aht10_init("aht10", &cfg);
    return RT_EOK;
}
INIT_ENV_EXPORT(rt_hw_aht10_port);
