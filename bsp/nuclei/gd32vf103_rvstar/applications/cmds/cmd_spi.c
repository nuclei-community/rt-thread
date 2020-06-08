#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_SPI) && defined(BSP_USING_SPI)

#if defined(RT_USING_SPI_MSD) && defined(RT_USING_DFS_ELMFAT)
#include <spi_msd.h>
#include <dfs_fs.h>

#define SPI_BUS_NAME    "spi1"
#define SPI_DEV_NAME    "spi10"
#define SPI_DEV_CSPIN   28
#define SDCARD_NAME     "sd0"

int mnt_init(void)
{
    rt_hw_spi_device_attach(SPI_BUS_NAME, SPI_DEV_NAME, SPI_DEV_CSPIN);
    msd_init(SDCARD_NAME, SPI_DEV_NAME);
    if (dfs_mount(SDCARD_NAME, "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("Mount \"/dev/sd0\" on \"/\"\n");
    }
    return 0;
}
INIT_ENV_EXPORT(mnt_init);

#endif /* defined(RT_USING_SPI_MSD) && defined(RT_USING_DFS_ELMFAT) */

#endif /*defined(RT_USING_SPI) && defined(BSP_USING_SPI)*/