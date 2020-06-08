#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_WDT) && defined(BSP_USING_WDT)

#define WDT_DEVICE_NAME    "wdt"    /* 看门狗设备名称 */

static rt_device_t wdg_dev;         /* 看门狗设备句柄 */

static void idle_hook(void)
{
    rt_uint16_t timeout;
    //rt_kprintf("feed the dog!\n ");

    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_GET_TIMELEFT, (void *)&timeout);
    // rt_kprintf("idle wdt timeout left: %u\n", timeout);
    if (timeout < 2) {
        /* 在空闲线程的回调函数里喂狗 */
        rt_kprintf("feed the dog @ %u!\n ", timeout);
        rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
    }
}

static int wdt_start(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    static rt_uint32_t timeout = 5, timeout2;        /* 溢出时间，单位：秒 */
    char *device_name = WDT_DEVICE_NAME; 

    /* 判断命令行参数是否给定了设备名称 */

    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    wdg_dev = rt_device_find(device_name);
    if (!wdg_dev)
    {
        rt_kprintf("find %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 初始化设备 */
    ret = rt_device_init(wdg_dev);
    if (ret != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n", device_name);
        return RT_ERROR;
    }
    
    /* 设置看门狗溢出时间 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, &timeout);
    if (ret != RT_EOK)
    {
        rt_kprintf("set %s timeout failed!\n", device_name);
        return RT_ERROR;
    }
    /* 启动看门狗 */
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_START, RT_NULL);
    if (ret != RT_EOK)
    {
        rt_kprintf("start %s failed!\n", device_name);
        return -RT_ERROR;
    }
    /* 设置空闲线程回调函数 */
    rt_thread_idle_sethook(idle_hook);

    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_GET_TIMELEFT, (void *)&timeout2);
    rt_kprintf("get wdt timeout left: %d\n", timeout2);
    timeout ++;
    rt_kprintf("Next timeout %d\n", timeout);
    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(wdt_start, wdt start);

static int wdt_stop(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    char *device_name = WDT_DEVICE_NAME; 

    /* 判断命令行参数是否给定了设备名称 */

    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    wdg_dev = rt_device_find(device_name);
    if (!wdg_dev)
    {
        rt_kprintf("find %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 初始化设备 */
    ret = rt_device_init(wdg_dev);
    
    ret = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_STOP, RT_NULL);
    return RT_EOK;
}

MSH_CMD_EXPORT(wdt_stop, wdt stop);


#endif /*defined(RT_USING_WDT) && defined(BSP_USING_WDT)*/