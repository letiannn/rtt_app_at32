/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-05-16     shelton      first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"
#include <ulog.h>

#include "drv_spi.h"
#include "dev_spi_flash_sfud.h"
#include "fal.h"
#include "dfs_fs.h"
#include "flashdb.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "main"
#endif
#define LOG_LVL LOG_LVL_DBG

static int ota_app_vtor_reconfig(void)
{
#define RT_APP_PORT_ADDR 0x08020000
#define NVIC_VTOR_MASK   0xFFFFFF80

    SCB->VTOR = RT_APP_PORT_ADDR;
    return 0;
}
INIT_BOARD_EXPORT(ota_app_vtor_reconfig);


int rt_spi_w25Q128_init(void)
{
    rt_hw_spi_device_attach("spi2", "spi20", GPIOB, GPIO_PINS_12);
    if (rt_sfud_flash_probe(FAL_USING_NOR_FLASH_DEV_NAME, "spi20"))
    {
        LOG_I("rt sfud flash probe success");
        return RT_EOK;
    }
    else
    {
        LOG_I("rt sfud flash probe fail");
        return -RT_ERROR;
    }
}
INIT_DEVICE_EXPORT(rt_spi_w25Q128_init);

int rt_fal_init(void)
{
    fal_init();
    return 0;
}
INIT_COMPONENT_EXPORT(rt_fal_init);

int dfs_mount_init(void)
{
#define FS_PARTITION_NAME "lfs"
    struct rt_device *lfs_mtd_dev = RT_NULL;
    lfs_mtd_dev = fal_mtd_nor_device_create(FS_PARTITION_NAME);
    if (!lfs_mtd_dev)
    {
        LOG_E("Can't create a mtd device on '%s' partition.", FS_PARTITION_NAME);
    }

    return 0;
}
INIT_COMPONENT_EXPORT(dfs_mount_init);


struct fdb_kvdb kvdb = { 0 };

uint8_t boot_count = 0;
static struct fdb_default_kv_node default_kv_table[] = {
    { "username", "flashdb", 0 },                          /* string KV */
    { "boot_count", &boot_count, sizeof(boot_count) },    /* int type KV */
};

int flashdb_init(void)
{
    struct fdb_default_kv default_kv = { 0 };
    default_kv.kvs = default_kv_table;
    default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);

    // fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, lock);
    // fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, unlock);

    int result = fdb_kvdb_init(&kvdb, "kvdb", "kvdb", &default_kv, NULL);

    if (result != FDB_NO_ERR)
    {
        return -1;
    }

    struct fdb_blob blob;
    fdb_kv_get_blob(&kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
    if (blob.saved.len > 0)
    {
        LOG_I("get the 'boot_count' value is %d", boot_count);
    }
    else
    {
        LOG_I("get the 'boot_count' failed");
    }
    boot_count++;
    fdb_kv_set_blob(&kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
    LOG_I("set the 'boot_count' value to %d", boot_count);

    return 0;
}
INIT_ENV_EXPORT(flashdb_init);

int main(void)
{
    while (1)
    {
        rt_thread_mdelay(5000);
    }
    return 0;
}
