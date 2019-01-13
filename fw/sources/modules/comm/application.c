/*
    BUT pneumobil - Copyright (C) 2019 Jakub Kaderka.

    This file is part of BUT pneumobil.

    BUT pneumobil is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    BUT pneumobil is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    application.c
 * @brief   Communication protocol, application layer
 *
 * @addtogroup modules-comm
 * @{
 */

#include <string.h>

#include "utils/assert.h"

#include "modules/log.h"
#include "modules/config.h"
#include "modules/comm/comm.h"
#include "modules/comm/can_transport.h"
#include "modules/comm/uart_transport.h"
#include "modules/comm/session.h"

#include "modules/comm/application.h"

#define CAN_IFACE Comm_CanSend
#define UART_IFACE Comm_UartSend

static void Commi_SendLogCan(const log_msg_t *log)
{
    ASSERT_NOT(log == NULL);
    Comm_SendLog(log, CAN_IFACE);
}

static void Commi_SendLogUart(const log_msg_t *log)
{
    ASSERT_NOT(log == NULL);
    Comm_SendLog(log, UART_IFACE);
}

/* ***************************************************************************
 * Application layer module local functions
 *************************************************************************** */
comm_error_t Comm_GetSysStatus(comm_sys_status_t *status)
{
    ASSERT_NOT(status == NULL);

    //TODO
    status->device_id = COMM_MY_ID;
    status->state = 0;
    status->uptime_s = 0;
    status->core_temp_c = 0;
    status->core_voltage_mv = 0;

    return COMM_OK;
}

comm_error_t Comm_SetLogMask(const comm_log_mask_t *payload)
{
    ASSERT_NOT(payload == NULL);

    /*
    switch (payload->interface) {
        case LOG_INTERFACE_CAN:
            Log_UpdateSubscription(Commi_LogCan, log_severity_t payload->severity);
            break;

        default:
            return COMM_ERR_INCORRECT_PARAM;
            break;
    }
    */
    return COMM_OK;
}

comm_error_t Comm_GetLogMask(uint8_t interface, comm_log_mask_t *response)
{
    ASSERT_NOT(response == NULL);

    /*
    switch (interface) {
        case LOG_INTERFACE_CAN:
            Log_GetSubscription(Commi_LogCan, response->severity);
            break;

        default:
            return COMM_ERR_INCORRECT_PARAM;
            break;
    }
    */
    return COMM_OK;
}

comm_error_t Comm_SetConfig(const comm_config_item_t *payload)
{
    ASSERT_NOT(payload == NULL);
    switch (payload->type) {
        case COMM_CONFIG_TYPE_BOOL:
            if (payload->id >= CONFIG_BOOL_COUNT) {
                return COMM_ERR_INCORRECT_PARAM;
            }
            Config_SetBool(payload->id, (bool) payload->u32);
            break;
        case COMM_CONFIG_TYPE_UINT:
            if (payload->id >= CONFIG_UINT_COUNT) {
                return COMM_ERR_INCORRECT_PARAM;
            }
            Config_SetUint(payload->id, (bool) payload->u32);
            break;

        default:
            return COMM_ERR_INCORRECT_PARAM;
            break;
    }
    return COMM_OK;
}

comm_error_t Comm_GetConfig(uint8_t id, uint8_t type,
        comm_config_item_t *response)
{
    ASSERT_NOT(response == NULL);
    response->id = id;
    response->type = type;

    switch (type) {
        case COMM_CONFIG_TYPE_BOOL:
            if (id >= CONFIG_BOOL_COUNT) {
                return COMM_ERR_INCORRECT_PARAM;
            }
            response->u32 = Config_GetBool(id);
            break;
        case COMM_CONFIG_TYPE_UINT:
            if (id >= CONFIG_UINT_COUNT) {
                return COMM_ERR_INCORRECT_PARAM;
            }
            response->u32 = Config_GetUint(id);
            break;

        default:
            return COMM_ERR_INCORRECT_PARAM;
            break;
    }
    return COMM_OK;
}

comm_error_t Comm_ResetConfig(void)
{
    Config_Reset();
    return COMM_OK;
}

comm_error_t Comm_LogMessage(uint16_t len, comm_node_t node,
        const comm_log_msg_t *payload)
{
#ifndef BOARD_HMI
    (void) len;
    (void) node;
    (void) payload;
#endif
    return COMM_OK;
}

/* ***************************************************************************
 * Application layer module global functions
 *************************************************************************** */
void Comm_Init(void)
{
    log_severity_t severity[LOG_SOURCE_COUNT];

    Comm_SessionInit();
    Comm_CanInit();
    Comm_UartInit();

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_WARNING;
    }
    severity[LOG_SOURCE_SYSTEM] = LOG_SEVERITY_INFO;
    severity[LOG_SOURCE_ECU] = LOG_SEVERITY_INFO;
    Log_Subscribe(Commi_SendLogCan, severity);

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_INFO;
    }
    Log_Subscribe(Commi_SendLogUart, severity);
}

/** @} */
