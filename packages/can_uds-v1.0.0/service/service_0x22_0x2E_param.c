/**
 * @file service_0x22_0x2E_param.c
 * @brief UDS service implementation for Parameter Management (0x22/0x2E).
 * @details - 0x22 Read Data By Identifier (RDBI)
 *          - 0x2E Write Data By Identifier (WDBI)
 * 
 * @author wdfk-prog ()
 * @version 1.0
 * @date 2025-11-29
 * 
 * @copyright Copyright (c) 2025  
 * 
 * @note    IMPORTANT:
 *          This file is an EXAMPLE integration. It depends on external modules:
 *          - parameter_manager.h (parameter_get, parameter_set)
 *          - general.h / general_extend.h (parameter objects)
 *          These functions are NOT implemented in the UDS library. 
 *          You must provide the backend implementation or adapt this file to 
 *          your specific non-volatile memory (NVM) manager.
 * @par Change Log:
 * Date       Version Author      Description
 * 2025-11-29 1.0     wdfk-prog   first version
 */
#include "rtt_uds_service.h"

/* 
 * External Dependencies 
 * (Ensure these headers exist in your project or replace with your own NVM API)
 */
//#include "parameter_manager.h"
//#include "general.h"
//#include "general_extend.h"
//#include "common_macro.h"

#define DBG_TAG "uds.param"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#ifdef UDS_ENABLE_PARAM_SVC

/* ==========================================================================
 * Configuration
 * ========================================================================== */

/** 
 * @brief Max buffer size for reading a single parameter.
 * @details Ensure this is large enough to hold the largest parameter structure 
 *          defined in your system.
 */
#ifndef UDS_PARAM_RDBI_BUF_SIZE
#define UDS_PARAM_RDBI_BUF_SIZE 32
#endif

#define PARAM_RDBI_BUF_SIZE UDS_PARAM_RDBI_BUF_SIZE

typedef struct sUdsRDBIArgsTag
{
    uint16_t did;
    uint16_t len;
    uint8_t data[UDS_PARAM_RDBI_BUF_SIZE];
} sUdsRDBIArgs_t;

sUdsRDBIArgs_t g_uds_param_rdbi_args[] = {
    { 0xF190, 17, { "UDS_RTTHREAD_TEST" } },
    { 0xF191, 0, { 0 } },
    { 0xF192, 0, { 0 } },
    { 0xF193, 0, { 0 } },
};

UDSErr_t helper_param_read(uint16_t did, char *buf, uint8_t *read_len)
{
    for (int i = 0; i < sizeof(g_uds_param_rdbi_args) / sizeof(sUdsRDBIArgs_t); i++)
    {
        if (g_uds_param_rdbi_args[i].did == did)
        {
            rt_memcpy(buf, &g_uds_param_rdbi_args[i].data, g_uds_param_rdbi_args[i].len);
            *read_len = g_uds_param_rdbi_args[i].len;
            return UDS_PositiveResponse;
        }
    }
    return UDS_NRC_RequestOutOfRange;
}

UDSErr_t helper_param_write(uint16_t did, char *buf, uint8_t len)
{
    for (int i = 0; i < sizeof(g_uds_param_rdbi_args) / sizeof(sUdsRDBIArgs_t); i++)
    {
        if (g_uds_param_rdbi_args[i].did == did)
        {
            rt_memcpy(&g_uds_param_rdbi_args[i].data, buf, len);
            g_uds_param_rdbi_args[i].len = len;
            return UDS_PositiveResponse;
        }
    }
    return UDS_NRC_RequestOutOfRange;
}

/* ==========================================================================
 * UDS Service Handlers
 * ========================================================================== */

/**
 * @brief  Handler for Service 0x22 (ReadDataByIdentifier).
 * @details Implements a lookup strategy:
 *          1. Try 'general_extend_obj' (Common/Global IDs).
 *          2. If not found, try 'general_obj' (Local/Legacy IDs).
 * 
 * @param  srv     UDS Server instance.
 * @param  data    Pointer to UDSRDBIArgs_t.
 * @param  context Unused.
 * @return UDS_PositiveResponse or NRC.
 */
static UDS_HANDLER(handle_rdbi)
{
    UDSRDBIArgs_t *args = (UDSRDBIArgs_t *)data;
    UDSErr_t result;
    uint16_t read_len = 0;

    /* Temporary buffer for parameter value (Stack allocated) */
    uint8_t temp_buf[PARAM_RDBI_BUF_SIZE];

    /* 1. Attempt read from Extended Object (Common IDs) */
    result = helper_param_read((uint16_t)args->dataId,
                               temp_buf,
                               &read_len); /* ReadLevel 0 */

    if (result == UDS_PositiveResponse)
    {
        /* Check if data fits in the UDS response PDU is handled by args->copy internally */
        return args->copy(srv, temp_buf, read_len);
    }

    /* Return the failure code (likely RequestOutOfRange if neither had it) */
    return result;
}

/**
 * @brief  Handler for Service 0x2E (WriteDataByIdentifier).
 * @details Uses the same lookup strategy as RDBI. 
 *          Writes are persisted to EEPROM (TRUE flag).
 * 
 * @param  srv     UDS Server instance.
 * @param  data    Pointer to UDSWDBIArgs_t.
 * @param  context Unused.
 * @return UDS_PositiveResponse or NRC.
 */
static UDS_HANDLER(handle_wdbi)
{
    UDSWDBIArgs_t *args = (UDSWDBIArgs_t *)data;
    UDSErr_t result;

    /* 1. Attempt write to Extended Object */
    result = helper_param_write((uint16_t)args->dataId,
                                (void *)args->data,
                                args->len); /* Persist to NVM */

    return result;
}

/* ==========================================================================
 * Service Registration
 * ========================================================================== */

/* 
 * Defines the registration functions:
 * - param_rdbi_node_register / unregister
 * - param_wdbi_node_register / unregister
 */
RTT_UDS_SERVICE_DEFINE_OPS(param_rdbi_node, UDS_EVT_ReadDataByIdent, handle_rdbi);
RTT_UDS_SERVICE_DEFINE_OPS(param_wdbi_node, UDS_EVT_WriteDataByIdent, handle_wdbi);

#endif /* UDS_ENABLE_PARAM_SVC */
