/**
 * @file service_0x36_0x37_0x38_file.c
 * @brief UDS File Transfer Service Implementation (Context-Based).
 */

#include "rtt_uds_service.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "rtconfig.h"

#ifdef RT_USING_FAL
#include <fal.h>
#endif

#define DBG_TAG "uds.download"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#ifdef UDS_ENABLE_DOWNLOAD_SVC

#ifndef UDS_BLACK_CHUNK_SIZE
#define UDS_BLACK_CHUNK_SIZE 1024
#endif

#define RID_REMOTE_FAL_NAME "ota"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* ==========================================================================
 * Helper Functions
 * ========================================================================== */

static uint32_t crc32_calc(uint32_t crc, const uint8_t *data, size_t len)
{
    crc = ~crc;
    while (len--)
    {
        crc ^= *data++;
        for (int k = 0; k < 8; k++)
        {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
    }
    return ~crc;
}

/* ==========================================================================
 * Service Handlers
 * ========================================================================== */

static UDS_HANDLER(handle_request_download)
{
    /* [Key Change] Get context from pointer */
    uds_download_service_t *ctx = (uds_download_service_t *)context;
    if (!ctx)
        return UDS_NRC_ConditionsNotCorrect;

    UDSRequestDownloadArgs_t *args = (UDSRequestDownloadArgs_t *)data;
    rt_kprintf("RequestDownload: addr:%x, size:%x, dfi:%d, blocklen:%d\n", args->addr, args->size, args->dataFormatIdentifier, args->maxNumberOfBlockLength);

    ctx->total_size = args->size;
    ctx->current_pos = 0;
    ctx->mode = DOWNLOAD_MODE_SYNC;
    ctx->current_crc = 0;

    return UDS_PositiveResponse;
}

static UDS_HANDLER(handle_transfer_data)
{
    uds_download_service_t *ctx = (uds_download_service_t *)context;
    if (!ctx)
        return UDS_NRC_ConditionsNotCorrect;

    UDSTransferDataArgs_t *args = (UDSTransferDataArgs_t *)data;
    rt_kprintf("Downloading: len:%d\n", args->len);

    UDSErr_t result = UDS_PositiveResponse;

    if (ctx->mode == DOWNLOAD_MODE_SYNC)
    {
        int res = fal_partition_write(ctx->fal_partition, ctx->current_pos, args->data, args->len);
        if(res == -1)
        {
            rt_kprintf("write fal partition failed!");
            return UDS_NRC_ResourceTemporarilyNotAvailable;
        }
        ctx->current_pos += args->len;
        ctx->current_crc = crc32_calc(ctx->current_crc, args->data, args->len);
        return UDS_PositiveResponse;
    }

    return UDS_NRC_ConditionsNotCorrect;
}

static UDS_HANDLER(handle_transfer_exit)
{
    uds_download_service_t *ctx = (uds_download_service_t *)context;
    if (!ctx)
        return UDS_NRC_ConditionsNotCorrect;

    UDSRequestTransferExitArgs_t *args = (UDSRequestTransferExitArgs_t *)data;
    rt_kprintf("Download exit: len:%d crc:%x alllen:%d crc:%x \n", args->len, *(uint32_t *)args->data, ctx->total_size, ctx->current_crc);

    if (ctx->mode == DOWNLOAD_MODE_SYNC)
    {
        ctx->mode = DOWNLOAD_MODE_IDLE;
    }

    UDSErr_t result = UDS_PositiveResponse;


    return UDS_PositiveResponse;
}

static UDS_HANDLER(handle_session_timeout)
{
    uds_download_service_t *ctx = (uds_download_service_t *)context;
    if (ctx)
    {
        LOG_W("download Session timeout!");
        ctx->mode = DOWNLOAD_MODE_IDLE;
    }
    return RTT_UDS_CONTINUE;
}

/* ==========================================================================
 * Public Registration API
 * ========================================================================== */

rt_err_t rtt_uds_download_service_mount(rtt_uds_env_t *env, uds_download_service_t *svc)
{
    if (!env || !svc)
        return -RT_EINVAL;

    /* Use configured name or default */
    svc->fal_partition = fal_partition_find(RID_REMOTE_FAL_NAME);

    /* Config Handlers */
    RTT_UDS_SERVICE_NODE_INIT(&svc->req_node, "down_req", UDS_EVT_RequestDownload, handle_request_download, svc, RTT_UDS_PRIO_NORMAL);
    RTT_UDS_SERVICE_NODE_INIT(&svc->data_node, "down_data", UDS_EVT_TransferData, handle_transfer_data, svc, RTT_UDS_PRIO_NORMAL);
    RTT_UDS_SERVICE_NODE_INIT(&svc->exit_node, "down_exit", UDS_EVT_RequestTransferExit, handle_transfer_exit, svc, RTT_UDS_PRIO_NORMAL);
    RTT_UDS_SERVICE_NODE_INIT(&svc->timeout_node, "down_tmo", UDS_EVT_SessionTimeout, handle_session_timeout, svc, RTT_UDS_PRIO_HIGHEST);

    /* Register */
    rtt_uds_service_register(env, &svc->req_node);
    rtt_uds_service_register(env, &svc->data_node);
    rtt_uds_service_register(env, &svc->exit_node);
    rtt_uds_service_register(env, &svc->timeout_node);

    return RT_EOK;
}

void rtt_uds_download_service_unmount(uds_download_service_t *svc)
{
    if (!svc)
        return;
    rtt_uds_service_unregister(&svc->req_node);
    rtt_uds_service_unregister(&svc->data_node);
    rtt_uds_service_unregister(&svc->exit_node);
    rtt_uds_service_unregister(&svc->timeout_node);
}

#endif /* UDS_ENABLE_FILE_SVC */
