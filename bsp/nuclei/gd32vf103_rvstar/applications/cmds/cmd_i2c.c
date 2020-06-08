#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_I2C) && defined(BSP_USING_I2C)

#ifdef PKG_USING_MPU6XXX
#include "sensor_inven_mpu6xxx.h"

int rt_hw_mpu6xxx_port(void)
{
    struct rt_sensor_config cfg;
    
    cfg.intf.dev_name = "i2c0";
    cfg.intf.user_data = (void *)MPU6XXX_ADDR_DEFAULT;
    cfg.irq_pin.pin = RT_PIN_NONE;

    rt_hw_mpu6xxx_init("mpu", &cfg);
    return 0;
}
INIT_APP_EXPORT(rt_hw_mpu6xxx_port);
#endif /* PKG_USING_MPU6XXX */

#endif /*defined(RT_USING_I2C) && defined(BSP_USING_I2C)*/