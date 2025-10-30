/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-01     RV           first version
 */


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define THREAD_PRIORITY         10
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        5


#define PIN_LED_B              GET_PIN(F, 11)      // PF11 :  LED_B        --> LED
#define PIN_LED_R              GET_PIN(F, 12)      // PF12 :  LED_R        --> LED

#define PIN_KEY0        GET_PIN(C, 0)     // PC0:  KEY0         --> KEY
#define PIN_KEY1        GET_PIN(C, 1)     // PC1 :  KEY1         --> KEY
#define PIN_KEY2        GET_PIN(C, 4)     // PC4 :  KEY2         --> KEY
#define PIN_WK_UP       GET_PIN(C, 5)     // PC5:  WK_UP        --> KEY

static rt_thread_t tid2 = RT_NULL;
static rt_sem_t dynamic_sem = RT_NULL;

static void recv_entry(void *parameter);

void key_callback(void *args)
{
    if (rt_pin_read(PIN_KEY0) == 0)
    {
        rt_sem_release(dynamic_sem);
    }
}

int main(void)
{
    rt_pin_mode(PIN_LED_R, PIN_MODE_OUTPUT);

    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(PIN_KEY0, PIN_IRQ_MODE_FALLING, key_callback, RT_NULL);
    rt_pin_irq_enable(PIN_KEY0, PIN_IRQ_ENABLE);

    dynamic_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_PRIO);

    tid2 = rt_thread_create("sem_recv",
                            recv_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);
}

static void recv_entry(void *parameter)
{
    while (1)
    {
        rt_sem_take(dynamic_sem, RT_WAITING_FOREVER);
        rt_kprintf("RUN IN sem_recv entry/Tick %d\r\n",rt_tick_get());
        rt_pin_write(PIN_LED_R, !rt_pin_read(PIN_LED_R));
    }
}


