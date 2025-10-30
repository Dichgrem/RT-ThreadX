/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-1      RV            first version
 */

#include <rtthread.h>
#include <stdio.h>
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define THREAD_PRIORITY         15
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static char usr_cmd[] = "memtrace";
static rt_thread_t tid = RT_NULL;

static void usr_entry(void *parameter);
extern int msh_exec(char *cmd, rt_size_t length);

void hexdump(const rt_uint8_t *p, rt_size_t len)
{
    unsigned char *buf = (unsigned char*)p;
    int i, j;

    rt_kprintf("Dump 0x%.8x %dBytes\n", (int)p, len);
    rt_kprintf("Offset      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (i=0; i<len; i+=16)
    {
        rt_kprintf("0x%08X: ", i+(int)p);

        for (j=0; j<16; j++)
        {
            if (i+j < len)
            {
                rt_kprintf("%02X ", buf[i+j]);
            }
            else
            {
                rt_kprintf("   ");
            }
        }
        rt_kprintf(" ");

        for (j=0; j<16; j++)
        {
            if (i+j < len)
            {
                rt_kprintf("%c", ((unsigned int)((buf[i+j]) - ' ') < 127u - ' ') ? buf[i+j] : '.');
            }
        }
        rt_kprintf("\n");
    }
}

int ramdump(int argc, char *argv[])
{
    int addr = 0;
    rt_size_t len = 0;

    if(argc != 3)
    {
        rt_kprintf("example: ramdump 0x00000000 len\n");
        return -1;
    }

    sscanf(argv[1],"%x",&addr);
    sscanf(argv[2],"%d",&len);

    hexdump((const rt_uint8_t *)addr, len);

    return 0;
}
MSH_CMD_EXPORT(ramdump, dump ram info.)

int main(void)
{

    tid = rt_thread_create("usr",
                            usr_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY - 1, THREAD_TIMESLICE);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}

static void usr_entry(void *parameter)
{
    int *usr_ptr = RT_NULL;
    rt_kprintf("enter usr_entry,rt_tick: %d \r\n", rt_tick_get());
    while (1)
    {
        usr_ptr = rt_malloc(1024 * 10);
        if (usr_ptr == RT_NULL)
        {
            rt_kprintf("exit usr_entry,rt_tick: %d \r\n", rt_tick_get());
            rt_kprintf("memory leak \r\n");
            msh_exec(usr_cmd, rt_strlen("memtrace"));
            return;
        }
        rt_memset(usr_ptr, 0X01, 1024 * 10);
        rt_thread_delay(50);
    }
}

