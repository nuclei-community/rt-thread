/*
 * 程序清单：这是一个 PIN 设备使用例程
 * 例程导出了 pin_beep_sample 命令到控制终端
 * 命令调用格式：pin_beep_sample
 * 程序功能：通过按键控制蜂鸣器对应引脚的电平状态控制蜂鸣器
*/

#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>

#if defined(RT_USING_PIN)

/* 引脚编号，通过查看设备驱动文件drv_gpio.c确定 */
#ifndef BEEP_PIN_NUM
    #define BEEP_PIN_NUM            1  /* PA1-LED_G */
#endif
#ifndef KEY0_PIN_NUM
    #define KEY0_PIN_NUM            43  /* PC11 */
#endif
#ifndef KEY1_PIN_NUM
    #define KEY1_PIN_NUM            0  /* PA0-WKUP */
#endif

void beep_on(void *args)
{
    rt_kprintf("turn on beep!\n");

    rt_pin_write(BEEP_PIN_NUM, PIN_HIGH);
}

void beep_off(void *args)
{
    rt_kprintf("turn off beep!\n");

    rt_pin_write(BEEP_PIN_NUM, PIN_LOW);
}

static void pin_beep_on(void)
{
    /* 蜂鸣器引脚为输出模式 */
    rt_pin_mode(BEEP_PIN_NUM, PIN_MODE_OUTPUT);
    /* 默认低电平 */
    rt_pin_write(BEEP_PIN_NUM, PIN_LOW);

    /* 按键0引脚为输入模式 */
    rt_pin_mode(KEY0_PIN_NUM, PIN_MODE_INPUT_PULLUP);
    /* 绑定中断，下降沿模式，回调函数名为beep_on */
    rt_pin_attach_irq(KEY0_PIN_NUM, PIN_IRQ_MODE_FALLING, beep_on, RT_NULL);
    /* 使能中断 */
    rt_pin_irq_enable(KEY0_PIN_NUM, PIN_IRQ_ENABLE);

    /* 按键1引脚为输入模式 */
    rt_pin_mode(KEY1_PIN_NUM, PIN_MODE_INPUT_PULLUP);
    /* 绑定中断，下降沿模式，回调函数名为beep_off */
    rt_pin_attach_irq(KEY1_PIN_NUM, PIN_IRQ_MODE_FALLING, beep_off, RT_NULL);
    /* 使能中断 */
    rt_pin_irq_enable(KEY1_PIN_NUM, PIN_IRQ_ENABLE);
}

static void pin_beep_off(void)
{
    /* 蜂鸣器引脚为输出模式 */
    rt_pin_mode(BEEP_PIN_NUM, PIN_MODE_OUTPUT);
    /* 默认低电平 */
    rt_pin_write(BEEP_PIN_NUM, PIN_LOW);

    
    /* 绑定中断，下降沿模式，回调函数名为beep_on */
    rt_pin_detach_irq(KEY0_PIN_NUM);
    /* 使能中断 */
    rt_pin_irq_enable(KEY0_PIN_NUM, PIN_IRQ_DISABLE);

    /* 绑定中断，下降沿模式，回调函数名为beep_off */
    rt_pin_detach_irq(KEY1_PIN_NUM);
    /* 使能中断 */
    rt_pin_irq_enable(KEY1_PIN_NUM, PIN_IRQ_DISABLE);
}

static void pin_read(int argc, char **argv)
{
    int rtpin;
    int pinval;
    if (argc > 1) {
        rtpin = atoi(argv[1]);
    } else {
        rtpin = KEY1_PIN_NUM;
    }
    rt_pin_mode(rtpin, PIN_MODE_INPUT);
    pinval = rt_pin_read(rtpin);

    rt_kprintf("Read pin %d value %d\n", rtpin, pinval);
}

static void pin_write(int argc, char **argv)
{
    int rtpin;
    int pinval = 0;
    if (argc > 1) {
        rtpin = atoi(argv[1]);
        if (argc > 2) {
            pinval = atoi(argv[2]);
        }
    } else {
        rtpin = BEEP_PIN_NUM;
    }
    rt_pin_mode(rtpin, PIN_MODE_OUTPUT);

    rt_pin_write(rtpin, pinval);

    rt_kprintf("Write pin %d value %d\n", rtpin, pinval);
}


/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(pin_beep_on, pin beep on);
MSH_CMD_EXPORT(pin_beep_off, pin beep off);
MSH_CMD_EXPORT(pin_read, pin read);
MSH_CMD_EXPORT(pin_write, pin write);

#endif /* defined(RT_USING_PIN) */