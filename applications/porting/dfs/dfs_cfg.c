
#include <rtthread.h>
#include <dfs.h>
#include <dfs_fs.h>
#include "dfs_romfs.h"

#ifdef RT_USING_DFS_MNTTABLE

const struct romfs_dirent _root_dirent[] =
{
    {ROMFS_DIRENT_DIR, "lfs", 0, 0},
};

const struct romfs_dirent romfs_root =
{
    ROMFS_DIRENT_DIR, "/", (rt_uint8_t *)_root_dirent, sizeof(_root_dirent) / sizeof(_root_dirent[0])
};

const struct dfs_mount_tbl mount_table[] = {
    {RT_NULL, 	"/", 		"rom", 0, &(romfs_root)},
    { "lfs", 	"/lfs",     "lfs", 0, 0 },
    { 0 },
};

#endif
