/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       HYT
 * 2022-06-01     RT-Thread    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

char cmd_recv[3];// 接收指令ID，LED2状态，LED3状态
char mag_send[7]={1};//发送信息，ID，LED2，K1，LED3，K2，电压整数，电压小数

/* 2.1 获取引脚编号 */
#define LED1 GET_PIN(A,0)
#define LED2 GET_PIN(A,1)
#define LED3 GET_PIN(A,4)
#define K1 GET_PIN(B,12)
#define K2 GET_PIN(B,13)

/* 4.1 定义串口设备句柄 */
rt_device_t uart3;

/* 2.5 引脚中断回调函数 */
void k_irq(void *args)
{
    int k = (int)args;//获取中断回调函数参数

    /* 根据参数判断K1或K2中断 */
    if(k==1)//如果为K1中断
    {
        /* 改变LED2状态 */
        rt_pin_write(LED2, 1-rt_pin_read(LED2));

        /* 4.6 打包数据并发送数据 */
        mag_send[1] = 1 - rt_pin_read(LED2);
        mag_send[2] = rt_pin_read(K1);
        rt_device_write(uart3, 0, &mag_send, sizeof(mag_send));
    }
    else//如果为K2中断
    {
        /* 改变LED3状态 */
        rt_pin_write(LED3, 1-rt_pin_read(LED3));

        /* 4.6 打包数据并发送数据 */
        mag_send[3] = 1 - rt_pin_read(LED3);
        mag_send[4] = rt_pin_read(K2);
        rt_device_write(uart3, 0, &mag_send, sizeof(mag_send));
    }
}

/* 4.5 串口接收回调函数 */
rt_err_t uart_recv_callback(rt_device_t dev, rt_size_t size)
{
    /* 打印接收设备及接收数据大小 */
    //rt_kprintf("%s, recv %d bytes\n", dev->parent.name, size);

    /* 4.7 读取串口设备数据 */
    rt_device_read(dev, 0, &cmd_recv, size);

    /* 解析接收数据，并执行相应动作 */
    if(cmd_recv[0] == 0x01)//判断ID是否为01
    {
        if(cmd_recv[1] == 0x01)//第1位为01
        {
            rt_pin_write(LED2, PIN_LOW);//点亮LED2
        }
        else //第1位不为01
        {
            rt_pin_write(LED2, PIN_HIGH);//熄灭LED2
        }
        if(cmd_recv[2] == 0x01)//第2位为01
        {
            rt_pin_write(LED3, PIN_LOW);//点亮LED3
        }
        else //第2位不为01
        {
            rt_pin_write(LED3, PIN_HIGH);//熄灭LED3
        }
    }

    return RT_EOK;
}

/* 1.4 线程入口函数 */
void t_io_entry(void *parameter)
{
    /* 2.2 设置引脚模式 */
    rt_pin_mode(LED1, PIN_MODE_OUTPUT);
    rt_pin_mode(LED2, PIN_MODE_OUTPUT);
    rt_pin_mode(LED3, PIN_MODE_OUTPUT);
    rt_pin_mode(K1, PIN_MODE_INPUT_PULLDOWN);
    rt_pin_mode(K1, PIN_MODE_INPUT_PULLDOWN);

    /* 2.3 绑定中断回调函数 */
    rt_pin_attach_irq(K1, PIN_IRQ_MODE_RISING_FALLING, k_irq, (void*)1);
    rt_pin_attach_irq(K2, PIN_IRQ_MODE_RISING_FALLING, k_irq, (void*)2);

    /* 2.4 使能引脚中断 */
    rt_pin_irq_enable(K1, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(K2, PIN_IRQ_ENABLE);

    /* 2.6 熄灭LED1、LED2和LED3 */
    rt_pin_write(LED1, PIN_HIGH);
    rt_pin_write(LED2, PIN_HIGH);
    rt_pin_write(LED3, PIN_HIGH);

    /* 2.7 LED1间隔0.5s闪烁3次 */
    for(int i=0; i<6; i++)
    {
        rt_pin_write(LED1, 1-rt_pin_read(LED1));
        rt_thread_mdelay(500);
    }

    /* 2.8 LED1间隔1s闪烁 */
    while(1)
    {
        rt_pin_write(LED1, 1-rt_pin_read(LED1));
        rt_thread_mdelay(1000);
    }
}

void t_ain_entry(void *parameter)
{
    /* 3.1 定义ADC设备句柄 */
    rt_adc_device_t adc_dev;
    rt_uint32_t adval, volt;

    /* 3.2 查找ADC设备 */
    adc_dev = (rt_adc_device_t) rt_device_find("adc1");

    /* 3.3 使能ADC设备 */
    rt_adc_enable(adc_dev, 4);

    while (1)
    {
        /* 3.4 读取采样值 */
        adval = rt_adc_read(adc_dev, 4);
        //rt_kprintf("the adval is :%d \n", adval);

        /* 3.5 转换为对应电压值，参考电压3.3V，AD分辨率12位，结果*100 */
        volt = adval * 330 / 4096;
        rt_kprintf("the voltage is :%d.%02d \n", volt / 100, volt % 100);

        /* 3.6 第5位电压整数，第6位电压小数 */
        mag_send[5] = volt / 100;
        mag_send[6] = volt % 100;

        rt_thread_mdelay(1000);
    }
}

void t_uart_entry(void *parameter)
{
    /* 4.2 查找串口设备 */
    uart3 = rt_device_find("uart3");

    /* 4.3 打开串口设备，DMA接收轮询发送 */
    rt_device_open(uart3, RT_DEVICE_FLAG_DMA_RX);

    /* 4.4 设置接收回调函数 */
    rt_device_set_rx_indicate(uart3, uart_recv_callback);
    while(1)
    {
        /* 4.6 打包数据并发送数据 */
        mag_send[1] = 1 - rt_pin_read(LED2);
        mag_send[3] = 1 - rt_pin_read(LED3);
        rt_device_write(uart3, 0, &mag_send, sizeof(mag_send));
        rt_thread_mdelay(1000);
    }
}

int main(void)
{
    /* 1.1 定义线程句柄 */
    rt_thread_t t_io, t_ain, t_uart;

    /* 1.2 创建线程 */
    t_io = rt_thread_create("tio", t_io_entry, RT_NULL, 1024, 10, 10);
    t_ain = rt_thread_create("tain", t_ain_entry, RT_NULL, 1024, 10, 10);
    t_uart = rt_thread_create("tuart", t_uart_entry, RT_NULL, 1024, 10, 10);

    /* 1.3 启动线程 */
    rt_thread_startup(t_io);
    rt_thread_startup(t_ain);
    rt_thread_startup(t_uart);

    return RT_EOK;
}



