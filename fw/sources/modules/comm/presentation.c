/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

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
 * @file    presentation.c
 * @brief   Communication protocol, presentation layer
 *
 * @addtogroup modules-comm
 * @{
 */

#include "utils/assert.h"
#include "modules/log.h"
#include "modules/config.h"

#include "modules/comm/application.h"
#include "modules/comm/presentation.h"

static comm_error_t Commi_SetLogMask(const comm_log_mask_t *payload)
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

static comm_error_t Commi_GetLogMask(uint8_t interface,
        comm_log_mask_t *response)
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

static comm_error_t Commi_SetConfig(const comm_config_item_t *payload)
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

static comm_error_t Commi_GetConfig(uint8_t id, uint8_t type,
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

static comm_error_t Commi_ResetConfig(void)
{
    Config_Reset();
    return COMM_OK;
}

static comm_error_t Commi_LogMessage(uint16_t len, comm_node_t node,
        const comm_log_msg_t *payload)
{
    //TODO
    return COMM_OK;
}

bool Comm_HandlePacket(const comm_packet_t *packet, comm_send_cb_t send_iface)
{
    comm_error_t retval = COMM_ERR_INCORRECT_LEN;
    bool replyRequired = true;
    static comm_packet_t response;

    ASSERT_NOT(packet == NULL || send_iface == NULL);

    response.len = 0;
    switch (packet->cmd.id) {
        /* Generic commands*/
        case COMM_CMD_SYSTEM_STATUS:
            if (packet->len == sizeof(comm_system_status_t)) {
            }
        break;
        case COMM_CMD_SET_LOG_MASK:
            if (packet->len == sizeof(comm_log_mask_t)) {
                retval = Commi_SetLogMask((comm_log_mask_t *) packet->payload);
            }
            break;
        case COMM_CMD_GET_LOG_MASK:
            if (packet->len == 1) {
                retval = Commi_GetLogMask(packet->payload[0],
                        (comm_log_mask_t *) response.payload);
                response.len = sizeof(comm_log_mask_t);
            }
            break;
        case COMM_CMD_SET_CONFIG:
            if (packet->len == sizeof(comm_config_item_t)) {
                retval = Commi_SetConfig((comm_config_item_t *) packet->payload);
            }
            break;
        case COMM_CMD_GET_CONFIG:
            if (packet->len == 2) {
                retval = Commi_GetConfig(packet->payload[0], packet->payload[1],
                        (comm_config_item_t *) response.payload);
                response.len = sizeof(comm_config_item_t);
            }
            break;
        case COMM_CMD_RESET_CONFIG:
            if (packet->len == 4) {
                if (*((uint32_t *)packet->payload) == 0xdeadbeef) {
                    retval = Commi_ResetConfig();
                } else {
                    retval = COMM_ERR_INCORRECT_PARAM;
                }
            }
            break;
        /*
        case COMM_CMD_GET_ALL_CONFIG:
            break;
        */
        case COMM_CMD_LOG_MESSAGE:
            if (packet->len >= 3) {
                retval = Commi_LogMessage(packet->len - 2, packet->node,
                        (comm_log_msg_t *) packet->payload);
                replyRequired = false;
            }
            break;

        /* Flash commands*/
        case COMM_CMD_FLASH_VERSION:
        break;
        case COMM_CMD_GET_FLASH_VERSION:
        break;
        case COMM_CMD_FLASH_MODE:
        break;
        case COMM_CMD_FLASH_INIT:
        break;
        case COMM_CMD_FLASH_DATA:
        break;
        case COMM_CMD_FLASH_FINISH:
        break;
        case COMM_CMD_REBOOT:
        break;

#ifdef BOARD_HMI
        /* ECU commands*/
        case COMM_CMD_CAR_STATE:
        break;
        case COMM_CMD_PNEU_STATE:
        break;
        case COMM_CMD_GPS_POSITION:
        break;
        case COMM_CMD_TEMPS:
        break;
        case COMM_CMD_SET_MODE:
        break;

        /* PSU commands*/
        case COMM_CMD_BATTERY:
        break;
        case COMM_CMD_CURRENTS:
        break;
#endif

        default:
            retval = COMM_ERR_UNSUPPORTED_CMD;
            break;
    }

    if (replyRequired) {
        response.len = 1;
        if (response.len == 0) {
            response.payload[0] = retval;
            response.cmd.response = 1;
            if (retval != COMM_OK) {
                response.cmd.error = true;
            }
        }

        if (Comm_SendPayload(&response, NULL, send_iface) != COMM_OK) {
            return false;
        }
    }

    return true;
}

/** @} */
