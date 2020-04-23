/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-22     hqfang       First version
 */

#include <drv_uart.h>

#ifdef RT_USING_SERIAL

#if !defined(BSP_USING_UART0) && !defined(BSP_USING_UART1)
    #error "Please define at least one BSP_USING_UARTx"
    /* this driver can be enabled at menuconfig -> 
    Hardware Drivers Config -> On-chip Peripheral Drivers -> Enable UART */
#endif

enum
{
#ifdef BSP_USING_UART0
    UART0_INDEX,
#endif
#ifdef BSP_USING_UART1
    UART1_INDEX,
#endif
#ifdef BSP_USING_UART2
    UART2_INDEX,
#endif
#ifdef BSP_USING_UART3
    UART3_INDEX,
#endif
#ifdef BSP_USING_UART4
    UART4_INDEX,
#endif
};

static struct hbird_uart_config uart_config[] =
{
#ifdef BSP_USING_UART0
    {
        "uart0",
        UART0,
        SOC_INT35_IRQn,
        SOC_INT36_IRQn,
    },
#endif
#ifdef BSP_USING_UART1
    {
        "uart1",
        UART1,
        SOC_INT43_IRQn,
        SOC_INT44_IRQn,
    },
#endif
};

static struct hbird_uart uart_obj[sizeof(uart_config) / sizeof(uart_config[0])] = {0};

static rt_err_t hbird_configure(struct rt_serial_device *serial,
                               struct serial_configure *cfg)
{
    struct hbird_uart *uart_obj;
    struct hbird_uart_config *uart_cfg;
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);

    uart_obj = (struct hbird_uart *) serial->parent.user_data;
    uart_cfg = uart_obj->config;
    RT_ASSERT(uart_cfg != RT_NULL);

    uart_init(uart_cfg->uart, cfg->baud_rate);

    switch (cfg->stop_bits)
    {
    case STOP_BITS_1:
        uart_config_stopbit(uart_cfg->uart, UART_STOP_BIT_1);
        break;
    case STOP_BITS_2:
        uart_config_stopbit(uart_cfg->uart, UART_STOP_BIT_2);
        break;
    default:
        uart_config_stopbit(uart_cfg->uart, UART_STOP_BIT_1);
        break;
    }

    return RT_EOK;
}

static rt_err_t hbird_control(struct rt_serial_device *serial, int cmd,
                             void *arg)
{
    struct hbird_uart *uart_obj;
    struct hbird_uart_config *uart_cfg;

    RT_ASSERT(serial != RT_NULL);
    uart_obj = (struct hbird_uart *) serial->parent.user_data;
    uart_cfg = uart_obj->config;
    RT_ASSERT(uart_cfg != RT_NULL);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        ECLIC_DisableIRQ(uart_cfg->rx_irqn);
        uart_disable_rxint(uart_cfg->uart);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        ECLIC_EnableIRQ(uart_cfg->rx_irqn);
        uart_enable_rxint(uart_cfg->uart);
        break;
    }

    return RT_EOK;
}

static int hbird_putc(struct rt_serial_device *serial, char ch)
{
    struct hbird_uart *uart_obj;
    struct hbird_uart_config *uart_cfg;

    RT_ASSERT(serial != RT_NULL);
    uart_obj = (struct hbird_uart *) serial->parent.user_data;
    uart_cfg = uart_obj->config;
    RT_ASSERT(uart_cfg != RT_NULL);

    uart_write(uart_cfg->uart, ch);

    return 1;
}

static int hbird_getc(struct rt_serial_device *serial)
{
    int ch;
    struct hbird_uart *uart_obj;
    struct hbird_uart_config *uart_cfg;

    RT_ASSERT(serial != RT_NULL);
    uart_obj = (struct hbird_uart *) serial->parent.user_data;
    uart_cfg = uart_obj->config;
    RT_ASSERT(uart_cfg != RT_NULL);

    ch = -1;
    if (! (uart_cfg->uart->RXFIFO & UART_RXFIFO_EMPTY)) {
        ch = (int)(uint8_t)(uart_cfg->uart->RXFIFO);
    }
    return ch;
}

static const struct rt_uart_ops hbird_uart_ops = { hbird_configure, hbird_control,
           hbird_putc, hbird_getc,
           RT_NULL
};

static void uart_rx_isr(struct rt_serial_device *serial)
{
    RT_ASSERT(serial != RT_NULL);

    rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
}

#ifdef BSP_USING_UART0

void eclic_irq35_handler(void)
{
    rt_interrupt_enter();

    uart_rx_isr(&uart_obj[UART0_INDEX].serial);

    rt_interrupt_leave();
}

#endif

#ifdef BSP_USING_UART1

void eclic_irq43_handler(void)
{
    rt_interrupt_enter();

    uart_rx_isr(&uart_obj[UART1_INDEX].serial);

    rt_interrupt_leave();
}

#endif

int rt_hw_uart_init(void)
{
    rt_size_t obj_num;
    int index;

    obj_num = sizeof(uart_obj) / sizeof(struct hbird_uart);
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    rt_err_t result = 0;

    for (index = 0; index < obj_num; index++)
    {
        /* init UART object */
        uart_obj[index].config = &uart_config[index];
        uart_obj[index].serial.ops = &hbird_uart_ops;
        uart_obj[index].serial.config = config;

        /* register UART device */
        result = rt_hw_serial_register(&uart_obj[index].serial,
                                       uart_obj[index].config->name,
                                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                                       &uart_obj[index]);
        RT_ASSERT(result == RT_EOK);
    }

    return result;
}

#endif /* RT_USING_SERIAL */

/******************** end of file *******************/
