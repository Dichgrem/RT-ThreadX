/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-10     hytha       the first version
 */
#ifndef APPLICATIONS_APP_USART_H_
#define APPLICATIONS_APP_USART_H_

#include <rtthread.h>
#include <rtdevice.h>

#define DATA_CMD_END    '\r' /* 结束位设置为 \r，即回车符 */
#define ONE_DATA_MAXLEN  80  /* 不定长数据的最大长度 */

rt_device_t serial;  //定义串口设备句柄
rt_sem_t s_recv;     //定义信号量句柄

rt_err_t uart_recv_callback(rt_device_t dev, rt_size_t size);
char uart_get_char(void);
void uart_get_string(char data[]);

#endif /* APPLICATIONS_APP_USART_H_ */
