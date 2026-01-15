/**
 * @file service_0x31_console.c
 * @brief Implementation of UDS Service 0x31 (Remote Console) - Context Based.
 */

#include "rtt_uds_service.h"
#include "fal.h"

#define DBG_TAG "uds.fal"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* ==========================================================================
 * Configuration
 * ========================================================================== */
#define RID_REMOTE_FAL_NAME "ota"

#define RID_REMOTE_FAL_ERASE 0xF000
#define RID_REMOTE_FAL_WRITE 0xF100
#define RID_REMOTE_FAL_READ 0xF200
#define REQUIRED_SEC_LEVEL 0x01

/* ==========================================================================
 * Service Handler
 * ========================================================================== */

static UDS_HANDLER(handle_remote_fal)
{
    uds_fal_service_t *ctx = (uds_fal_service_t *)context;
    if (!ctx)
        return UDS_NRC_ConditionsNotCorrect;

    UDSRoutineCtrlArgs_t *args = (UDSRoutineCtrlArgs_t *)data;

    rt_kprintf("RoutineCtrl: type:%x, size:%x, id:%x, len:%d\n", args->ctrlType, args->id, args->len);
    /* 1. Session Check */
    if (srv->sessionType != UDS_LEV_DS_EXTDS && srv->sessionType != UDS_LEV_DS_PRGS)
        return UDS_NRC_ServiceNotSupportedInActiveSession;

    /* 2. Security Check */
    if (srv->securityLevel < REQUIRED_SEC_LEVEL)
        return UDS_NRC_SecurityAccessDenied;

    /* 3. Validate Request */
    if (args->ctrlType != UDS_LEV_RCTP_STR)
        return UDS_NRC_SubFunctionNotSupported;

    if (args->id != RID_REMOTE_FAL_ERASE && args->id != RID_REMOTE_FAL_WRITE && args->id != RID_REMOTE_FAL_READ)
        return UDS_NRC_RequestOutOfRange;

    if (args->len == 0)
        return UDS_NRC_IncorrectMessageLengthOrInvalidFormat;

    if(args->id == RID_REMOTE_FAL_ERASE)
    {
        int ret = fal_partition_erase_all(ctx->fal_partition);
        if (ret == -1)
            return UDS_NRC_GeneralReject;
    }

    /* 7. Send Response */
    if (args->copyStatusRecord)
    {
        /* Send buffer content */
        return args->copyStatusRecord(srv, (uint8_t *)&args->id, sizeof(uint16_t));
    }

    return UDS_PositiveResponse;
}

/* ==========================================================================
 * Public API
 * ========================================================================== */

rt_err_t rtt_uds_fal_service_mount(rtt_uds_env_t *env, uds_fal_service_t *svc)
{
    if (!env || !svc)
        return -RT_EINVAL;

    /* Use configured name or default */
    const char *dev_name = svc->dev_name ? svc->dev_name : RID_REMOTE_FAL_NAME;

    svc->fal_partition = fal_partition_find(dev_name);

    /* 2. Configure UDS Handler */
    RTT_UDS_SERVICE_NODE_INIT(&svc->service_node,
                              "fal_exec",
                              UDS_EVT_RoutineCtrl,
                              handle_remote_fal,
                              svc, /* Context binding */
                              RTT_UDS_PRIO_NORMAL);

    /* 3. Register to UDS Core */
    return rtt_uds_service_register(env, &svc->service_node);
}

void rtt_uds_fal_service_unmount(uds_fal_service_t *svc)
{
    if (!svc)
        return;

    /* Unregister from UDS */
    rtt_uds_service_unregister(&svc->service_node);

    LOG_I("Fal Service Unmounted");
}

