#include <rtthread.h>
#include <rtdevice.h>
#include "ulog.h"
#include "ulog_be.h"

#define LOG_FILE_PATH "/lfs/log"
#define FILE_MAX_NUM  5
#define FILE_SIZE     5 * 1024
#define BUFF_SIZE     256

typedef struct log_file_backend
{
    struct ulog_file_be *file_be;   // 文件后端结构体
    const char *name;               // 后端名称
    const char *dir_path;           // 文件路径
    rt_size_t max_num;              // 文件最大数量
    rt_size_t max_size;             // 文件最大大小
    rt_size_t buf_size;             // 缓存大小
    ulog_backend_filter_t filter;   // 过滤器
} log_file_backend_t;

static struct ulog_file_be anormal_log_file = { 0 };


rt_bool_t ulog_backend_filter(struct ulog_backend *backend, rt_uint32_t level, const char *tag, rt_bool_t is_raw, const char *log, rt_size_t len)
{
    if (level <= LOG_LVL_WARNING)
        return RT_TRUE;
    else
        return RT_FALSE;
}

static log_file_backend_t g_log_file_backend_tbl[] = {
    { &anormal_log_file, "warn", LOG_FILE_PATH, FILE_MAX_NUM, FILE_SIZE, BUFF_SIZE, ulog_backend_filter },
};

int ulog_file_backend_create(void)
{
    for (int i = 0; i < sizeof(g_log_file_backend_tbl) / sizeof(g_log_file_backend_tbl[0]); i++)
    {
        log_file_backend_t *log_file_be = (log_file_backend_t *)&g_log_file_backend_tbl[i];

        ulog_file_backend_init(log_file_be->file_be,
                               log_file_be->name,
                               log_file_be->dir_path,
                               log_file_be->max_num,
                               log_file_be->max_size,
                               log_file_be->buf_size);

        ulog_file_backend_enable(log_file_be->file_be);

        ulog_backend_set_filter(&log_file_be->file_be->parent, log_file_be->filter);
    }
    return 0;
}
INIT_PREV_EXPORT(ulog_file_backend_create);

static void cmd_log_file_backend(uint8_t argc, char **argv)
{
#define FILE_CMD_LIST    0
#define FILE_CMD_DEINIT  1
#define FILE_CMD_CONTROL 2

    size_t i = 0;

    const char *help_info[] = {
        [FILE_CMD_LIST] = "ulog_be_cmd list             - Prints the back-end status of all files.",
        [FILE_CMD_DEINIT] = "ulog_be_cmd deinit   [name]  - Deinit ulog file backend [name].",
        [FILE_CMD_CONTROL] = "ulog_be_cmd control  [name]  - Control ulog file backend [name] [enable/disable].",
    };

    const char *operator= argv[1];
    if (argc < 2)
    {
        rt_kprintf("Usage:\n");
        for (i = 0; i < sizeof(help_info) / sizeof(char *); i++)
        {
            rt_kprintf("%s\n", help_info[i]);
        }
        rt_kprintf("\n");
        return;
    }
    else
    {
        if (!rt_strcmp(operator, "list"))
        {
            const char *item_title = "file_be";
            int maxlen = RT_NAME_MAX;

            rt_kprintf("%-*.*s init_state\n", maxlen, maxlen, item_title);
            rt_kprintf("-------- ----------\n");
            for (uint8_t i = 0; i < sizeof(g_log_file_backend_tbl) / sizeof(g_log_file_backend_tbl[0]); i++)
            {
                rt_kprintf("%-*.*s", maxlen, maxlen, g_log_file_backend_tbl[i].name);
                ulog_backend_t file_be = ulog_backend_find(g_log_file_backend_tbl[i].name);
                if (file_be != RT_NULL)
                {
                    rt_kprintf("  init");
                }
                else
                {
                    rt_kprintf("  deinit ");
                }
                rt_kprintf("\n");
            }
        }
        else if (!rt_strcmp(operator, "deinit"))
        {
            if (argc < 3)
            {
                rt_kprintf("Usage:\n");
                rt_kprintf("Deinit ulog file backend [name]\n");
                return;
            }
            const char *operator= argv[2];
            const char *name = RT_NULL;
            uint8_t i;
            for (i = 0; i < sizeof(g_log_file_backend_tbl) / sizeof(g_log_file_backend_tbl[0]); i++)
            {
                if (!rt_strcmp(operator, g_log_file_backend_tbl[i].name))
                {
                    name = g_log_file_backend_tbl[i].name;
                    break;
                }
                else
                {
                    continue;
                }
            }
            if (name != RT_NULL)
            {
                ulog_file_backend_deinit(g_log_file_backend_tbl[i].file_be);
                ulog_file_backend_disable(g_log_file_backend_tbl[i].file_be);
                rt_kprintf("The file backend %s is deinit\n", operator);
            }
            else
            {
                rt_kprintf("File backend %s not found\n", operator);
                return;
            }
        }
        else if (!rt_strcmp(operator, "control"))
        {
            const char *operator= argv[2];
            const char *flag = argv[3];
            const char *name = RT_NULL;
            uint8_t i;
            if (argc < 4)
            {
                rt_kprintf("Usage:\n");
                rt_kprintf("control ulog file backend [name] [enable/disable]\n");
                return;
            }

            for (i = 0; i < sizeof(g_log_file_backend_tbl) / sizeof(g_log_file_backend_tbl[0]); i++)
            {
                if (!rt_strcmp(operator, g_log_file_backend_tbl[i].name))
                {
                    name = g_log_file_backend_tbl[i].name;
                    break;
                }
                else
                {
                    continue;
                }
            }
            if (name != RT_NULL)
            {
                if (!rt_strcmp(flag, "disable"))
                {
                    ulog_file_backend_disable(g_log_file_backend_tbl[i].file_be);
                    rt_kprintf("The file backend %s is disabled\n", operator);
                }
                else if (!rt_strcmp(flag, "enable"))
                {
                    ulog_file_backend_enable(g_log_file_backend_tbl[i].file_be);
                    rt_kprintf("The file backend %s is enable\n", operator);
                }
                else
                {
                    rt_kprintf("Usage:\n");
                    rt_kprintf("control ulog file backend [name] [enable/disable]\n");
                    return;
                }
            }
            else
            {
                rt_kprintf("File backend %s not found\n", operator);
                return;
            }
        }
        else
        {
            rt_kprintf("Usage:\n");
            for (i = 0; i < sizeof(help_info) / sizeof(char *); i++)
            {
                rt_kprintf("%s\n", help_info[i]);
            }
            rt_kprintf("\n");
            return;
        }
    }
}
MSH_CMD_EXPORT_ALIAS(cmd_log_file_backend, ulog_be_cmd, ulog file cmd);

