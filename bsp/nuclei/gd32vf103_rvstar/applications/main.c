/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-15     hqfang       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <nuclei_sdk_hal.h>

#define THREAD_PRIORITY 19
#define THREAD_STACK_SIZE 396
#define THREAD_TIMESLICE 5
#define THREAD_NUM       2
//#define APP_DEBUG_PRINT

/*　Align stack when using static thread　*/
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread_stack[THREAD_NUM][THREAD_STACK_SIZE];
static struct rt_thread tid[THREAD_NUM];

/* Thread entry function */
static void thread_entry(void *parameter)
{
    rt_uint32_t count = 0;

    while (1)
    {
#ifdef APP_DEBUG_PRINT
        rt_kprintf("thread %d count: %d\n", (rt_uint32_t)parameter, count++);
#endif
        rt_thread_mdelay(500);
        if ((rt_uint32_t)parameter < (LEDn - 1))
        {
            gd_rvstar_led_toggle((rt_uint32_t)parameter);
        }
    }
}

/* Thread demo */
int create_thread_demo(void)
{
    int i;
    static char tname[9] = "thread";

    for (i = 0; i < THREAD_NUM; i ++)
    {
        /* Create static threads */
        tname[6] = i / 10 + '0';
        tname[7] = i % 10 + '0';
        tname[8] = '\0';
        rt_thread_init(&tid[i], tname, thread_entry, (void *)i, thread_stack[i],
                       THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    }

    /* Startup threads  */
    for (i = 0; i < THREAD_NUM; i ++)
    {
        rt_thread_startup(&tid[i]);
    }

    return 0;
}

int main(void)
{
    rt_uint32_t count = 0;

    create_thread_demo();
    for (int i = 0; i < LEDn; i ++)
    {
        gd_rvstar_led_init(i);
    }


    while (1)
    {
#ifdef APP_DEBUG_PRINT
        rt_kprintf("Main thread count: %d\n", count++);
#endif
        rt_thread_mdelay(1000);
        gd_rvstar_led_toggle(LED3);
    }
}

/******************** end of file *******************/

