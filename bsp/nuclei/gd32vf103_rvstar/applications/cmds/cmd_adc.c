#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_ADC) && defined(BSP_USING_ADC)

#define ADC_DEV_NAME        "adc0"      /* ADC 设备名称 */
#define ADC_TMP_CHANNEL     16           /* ADC 通道 */
#define ADC_REF_CHANNEL     17           /* ADC 通道 */
#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */

static int adc_vol_sample(int argc, char *argv[])
{
    rt_adc_device_t adc_dev;
    rt_uint32_t value, vol;
    float temp;
    rt_err_t ret = RT_EOK;
    char *adc_name; 

    /* 查找设备 */
    if (argc > 1) {
        adc_name = argv[1];
    } else {
        adc_name = ADC_DEV_NAME;
    }
    adc_dev = (rt_adc_device_t)rt_device_find(adc_name);
    if (adc_dev == RT_NULL)
    {
        rt_kprintf("adc sample run failed! can't find %s device!\n", adc_name);
        return RT_ERROR;
    }

    /* 使能设备 */
    ret = rt_adc_enable(adc_dev, ADC_REF_CHANNEL);

    /* 读取采样值 */
    value = rt_adc_read(adc_dev, ADC_REF_CHANNEL);
    rt_kprintf("the value is :%d \n", value);

    /* 转换为对应电压值 */
    vol = value * REFER_VOLTAGE / CONVERT_BITS;
    rt_kprintf("the voltage is :%d.%02d \n", vol / 100, vol % 100);

    /* 关闭通道 */
    ret = rt_adc_disable(adc_dev, ADC_REF_CHANNEL);

    /* 使能设备 */
    ret = rt_adc_enable(adc_dev, ADC_TMP_CHANNEL);

    /* 读取采样值 */
    value = rt_adc_read(adc_dev, ADC_TMP_CHANNEL);
    rt_kprintf("the value is :%d \n", value);

    /* 转换为对应温度值 */
    temp = (1.42 - value *3.3 /4096) * 1000 / 4.3 + 25;
    rt_kprintf("the temperature is %d.%02d \n", (int)temp, \
                (int)(100 * (temp - (float)(int)temp)));

    /* 关闭通道 */
    ret = rt_adc_disable(adc_dev, ADC_TMP_CHANNEL);


    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(adc_vol_sample, adc voltage convert sample);

#endif /*defined(RT_USING_ADC) && defined(BSP_USING_ADC)*/