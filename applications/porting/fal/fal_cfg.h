#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <rtconfig.h>
#include <board.h>

#define ONCHIP_FLASH_DEV_NAME                       "onchip_flash"
#define NOR_FLASH_DEV_NAME                          "w25q64"

#define FAL_ONCHIP_START_ADDR_boot                  0
#define FAL_ONCHIP_MAX_LEN_boot                     (128*1024)
#define FAL_ONCHIP_START_ADDR_app                   131072
#define FAL_ONCHIP_MAX_LEN_app                      (512*1024)
#define FAL_ONCHIP_START_ADDR_sysinfo               655360
#define FAL_ONCHIP_MAX_LEN_sysinfo                  (4*1024)
#define FAL_ONCHIP_START_ADDR_reserved              659456
#define FAL_ONCHIP_MAX_LEN_reserved                 (16*1024)

#define FAL_NORFLASH_START_ADDR_factory             0
#define FAL_NORFLASH_MAX_LEN_factory                (512*1024)
#define FAL_NORFLASH_START_ADDR_ota                 524288
#define FAL_NORFLASH_MAX_LEN_ota                    (512*1024)
#define FAL_NORFLASH_START_ADDR_kvdb                1048576
#define FAL_NORFLASH_MAX_LEN_kvdb                   (512*1024)
#define FAL_NORFLASH_START_ADDR_tsdb                1572864
#define FAL_NORFLASH_MAX_LEN_tsdb                   (512*1024)
#define FAL_NORFLASH_START_ADDR_lfs                 2097152
#define FAL_NORFLASH_MAX_LEN_lfs                    (2048*1024)
#define FAL_NORFLASH_START_ADDR_userdata            4194304
#define FAL_NORFLASH_MAX_LEN_userdata               (512*1024)
#define FAL_NORFLASH_START_ADDR_log                 4718592
#define FAL_NORFLASH_MAX_LEN_log                    (2048*1024)
#define FAL_NORFLASH_START_ADDR_reserved            6815744
#define FAL_NORFLASH_MAX_LEN_reserved               (1024*1024)

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev at32_onchip_flash;
extern struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &at32_onchip_flash,                                              \
    &nor_flash0,                                                     \
}

/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                               \
{                                                                    \
    {FAL_PART_MAGIC_WORD,   "boot"        , "onchip_flash",            0,       131072, 0}, \
    {FAL_PART_MAGIC_WORD,   "app"         , "onchip_flash",       131072,       524288, 0}, \
    {FAL_PART_MAGIC_WORD,   "sysinfo"     , "onchip_flash",       655360,         4096, 0}, \
    {FAL_PART_MAGIC_WORD,   "reserved"    , "onchip_flash",       659456,        16384, 0}, \
    {FAL_PART_MAGIC_WORD,   "factory"     , "w25q64"      ,            0,       524288, 0}, \
    {FAL_PART_MAGIC_WORD,   "ota"         , "w25q64"      ,       524288,       524288, 0}, \
    {FAL_PART_MAGIC_WORD,   "kvdb"        , "w25q64"      ,      1048576,       524288, 0}, \
    {FAL_PART_MAGIC_WORD,   "tsdb"        , "w25q64"      ,      1572864,       524288, 0}, \
    {FAL_PART_MAGIC_WORD,   "lfs"         , "w25q64"      ,      2097152,      2097152, 0}, \
    {FAL_PART_MAGIC_WORD,   "userdata"    , "w25q64"      ,      4194304,       524288, 0}, \
    {FAL_PART_MAGIC_WORD,   "log"         , "w25q64"      ,      4718592,      2097152, 0}, \
    {FAL_PART_MAGIC_WORD,   "reserved"    , "w25q64"      ,      6815744,      1048576, 0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
