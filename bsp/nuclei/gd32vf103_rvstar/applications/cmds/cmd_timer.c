#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_HWTIMER) && defined(BSP_USING_HWTIMER)

#define HWTIMER_DEV_NAME   "timer1"     /* 定时器名称 */
rt_device_t hw_dev;                     /* 定时器设备句柄 */
rt_hwtimer_mode_t mode;         /* 定时器模式 */
rt_uint32_t freq = 10000;       /* 计数频率 */
rt_uint32_t entercnt = 0;       /* ISR CNT */

/* 定时器超时回调函数 */
static rt_err_t timeout_cb(rt_device_t dev, rt_size_t size)
{
    rt_kprintf("this is hwtimer timeout callback function - %d!\n", entercnt++);
    rt_kprintf("ms is :%d !\n", (rt_tick_get() * 1000 / RT_TICK_PER_SECOND));

    return 0;
}

static int hwtimer_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    rt_hwtimerval_t timeout_s;      /* 定时器超时值 */
    rt_device_t hw_dev = RT_NULL;   /* 定时器设备句柄 */
    rt_hwtimer_mode_t mode;         /* 定时器模式 */
    char *hwtimer_name; 

    /* 查找定时器设备 */
    if (argc > 1) {
        hwtimer_name = argv[1];
    } else {
        hwtimer_name = HWTIMER_DEV_NAME;
    }
    entercnt = 0;
    rt_kprintf("Open device %s\n", hwtimer_name);
    hw_dev = rt_device_find((const char *)hwtimer_name);
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", hwtimer_name);
        return RT_ERROR;
    }

    /* 以读写方式打开设备 */
    ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed - %d ! \n", hwtimer_name, ret);
        return ret;
    }

    /* 设置计数频率(默认1Mhz或支持的最小计数频率) */
    //rt_device_control(hw_dev, HWTIMER_CTRL_FREQ_SET, &freq);

    /* 设置超时回调函数 */
    rt_device_set_rx_indicate(hw_dev, timeout_cb);

    /* 设置模式为周期性定时器 */
    mode = HWTIMER_MODE_PERIOD;
    //mode = HWTIMER_MODE_ONESHOT;
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        rt_kprintf("set mode failed! ret is :%d\n", ret);
        return ret;
    }

    /* 设置定时器超时值为1s并启动定时器 */
    timeout_s.sec = 0;      /* 秒 */
    timeout_s.usec = 500000;     /* 微秒 */

    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }

    /* 延时1000ms */
    rt_thread_mdelay(3000);

    /* 读取定时器当前值 */
    rt_device_read(hw_dev, 0, &timeout_s, sizeof(timeout_s));
    rt_kprintf("Read: Sec = %d, Usec = %d\n", timeout_s.sec, timeout_s.usec);

    rt_device_close(hw_dev);

    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(hwtimer_sample, hwtimer sample);

#endif /*defined(RT_USING_HWTIMER) && defined(BSP_USING_HWTIMER)*/