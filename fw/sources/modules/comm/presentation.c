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

#include <string.h>
#include "utils/assert.h"
#include "modules/log.h"

#include "modules/comm/application.h"
#include "modules/comm/presentation.h"

bool Comm_SendLog(comm_send_cb_t iface, const log_msg_t *msg)
{
    comm_error_t res;
    comm_packet_t packet;
    comm_priority_t priority = COMM_PRIORITY_LOG;
    comm_log_msg_t *log = (comm_log_msg_t *) &packet.payload;

    if (msg->severity == LOG_SEVERITY_DEBUG || msg->severity == LOG_SEVERITY_INFO) {
        priority = COMM_PRIORITY_DEBUG;
    }

    packet.node = COMM_NODE_BROADCAST;
    packet.priority = priority;
    packet.cmd.intval = 0;
    packet.cmd.id = COMM_CMD_LOG_MESSAGE;
    packet.len = strlen(msg->msg) + 2;
    log->severity = msg->severity;
    log->source = msg->src;
    strncpy((char *)log->message, msg->msg, LOG_MSG_LEN);

    res = Comm_SendPayload(&packet, NULL, iface);
    if (res == COMM_OK) {
        return true;
    }
    return false;
}

bool Comm_SendPacket(comm_cmd_id_t cmd, const uint8_t *data, uint8_t len)
{
    comm_error_t res;
    comm_packet_t packet;

    ASSERT_NOT(data == NULL || len == 0);

    packet.node = COMM_NODE_BROADCAST;
    packet.priority = COMM_PRIORITY_NORMAL;
    packet.cmd.intval = 0;
    packet.cmd.id = cmd;
    packet.len = len;
    memcpy(packet.payload, data, len);

    res = Comm_SendPayload(&packet, NULL, IFACE_CAN);
    if (res == COMM_OK) {
        return true;
    }
    return false;
}

bool Comm_SendPacketCmd(comm_node_t target, comm_cmd_id_t cmd,
        const uint8_t *data, uint8_t len, uint8_t *response, uint8_t rsp_len)
{
    comm_error_t res;
    comm_packet_t packet;
    comm_packet_t responsePacket;

    packet.node = target;
    packet.priority = COMM_PRIORITY_CRITICAL;
    packet.cmd.intval = 0;
    packet.cmd.id = cmd;
    packet.len = len;
    memcpy(packet.payload, data, len);

    responsePacket.len = 0;

    res = Comm_SendPayload(&packet, &responsePacket, IFACE_CAN);
    if (res != COMM_OK) {
        return false;
    }

    if (response == NULL || rsp_len == 0) {
        return true;
    }

    if (responsePacket.len != rsp_len) {
        Log_Warn(LOG_SOURCE_COMM,
                "Received incorrect len response, wanted %d, got %d", rsp_len,
                responsePacket.len);
        return false;
    }

    memcpy(response, responsePacket.payload, rsp_len);
    return true;
}

bool Comm_HandlePacket(comm_node_t dest, const comm_packet_t *packet,
        comm_send_cb_t send_iface)
{
    comm_error_t retval = COMM_ERR_INCORRECT_LEN;
    bool replyRequired = true;
    static comm_packet_t response;

    ASSERT_NOT(packet == NULL || send_iface == NULL);

    if (dest != COMM_MY_ID || dest != COMM_NODE_BROADCAST) {
        Log_Warn(LOG_SOURCE_COMM, "Received message for node %x, ignoring",
                dest);
        return false;
    }

    response.len = 0;
    switch (packet->cmd.id) {
        /* Generic commands*/
        case COMM_CMD_SYSTEM_STATUS:
            if (packet->len == 0) {
                retval = Comm_GetSysStatus((comm_sys_status_t *) response.payload);
                response.len = sizeof(comm_sys_status_t);
            }
            break;
        case COMM_CMD_SET_LOG_MASK:
            if (packet->len == sizeof(comm_log_mask_t)) {
                retval = Comm_SetLogMask((comm_log_mask_t *) packet->payload);
            }
            break;
        case COMM_CMD_GET_LOG_MASK:
            if (packet->len == 1) {
                retval = Comm_GetLogMask(packet->payload[0],
                        (comm_log_mask_t *) response.payload);
                response.len = sizeof(comm_log_mask_t);
            }
            break;
        case COMM_CMD_SET_CONFIG:
            if (packet->len == sizeof(comm_config_item_t)) {
                retval = Comm_SetConfig((comm_config_item_t *) packet->payload);
            }
            break;
        case COMM_CMD_GET_CONFIG:
            if (packet->len == 2) {
                retval = Comm_GetConfig(packet->payload[0], packet->payload[1],
                        (comm_config_item_t *) response.payload);
                response.len = sizeof(comm_config_item_t);
            }
            break;
        case COMM_CMD_RESET_CONFIG:
            if (packet->len == 4) {
                if (*((uint32_t *)packet->payload) == 0xdeadbeef) {
                    retval = Comm_ResetConfig();
                } else {
                    retval = COMM_ERR_INCORRECT_PARAM;
                }
            }
            break;
        case COMM_CMD_LOG_MESSAGE:
            if (packet->len >= 3) {
                retval = Comm_LogMessage(packet->len - 2, packet->node,
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
            if (packet->len == sizeof(comm_car_state_t)) {
                Comm_CarState((comm_car_state_t *) packet->payload);
                replyRequired = false;
            }
            break;
        case COMM_CMD_CAR_IO:
            if (packet->len == sizeof(comm_car_io_t)) {
                Comm_CarIO((comm_car_io_t *) packet->payload);
                replyRequired = false;
            }
            break;
        case COMM_CMD_PNEU_STATE:
            if (packet->len == sizeof(comm_pneu_state_t)) {
                Comm_PneuState((comm_pneu_state_t *) packet->payload);
                replyRequired = false;
            }
            break;
        /* PSU commands*/
        case COMM_CMD_BATTERY_STATE:
            if (packet->len == sizeof(comm_battery_state_t)) {
                Comm_BatteryState((comm_battery_state_t *) packet->payload);
                replyRequired = false;
            }
            break;
        case COMM_CMD_PSU_CURRENT:
            if (packet->len == sizeof(comm_psu_current_t)) {
                Comm_PSUCurrent((comm_psu_current_t *) packet->payload);
                replyRequired = false;
            }
            break;
        case COMM_CMD_PSU_VOLTAGE:
            if (packet->len == sizeof(comm_psu_voltage_t)) {
                Comm_PSUVoltage((comm_psu_voltage_t *) packet->payload);
                replyRequired = false;
            }
            break;
#else
        case COMM_CMD_CAR_STATE:
        case COMM_CMD_CAR_IO:
        case COMM_CMD_PNEU_STATE:
        case COMM_CMD_GPS_POSITION:
        case COMM_CMD_TEMPS:
        case COMM_CMD_BATTERY_STATE:
        case COMM_CMD_PSU_CURRENT:
        case COMM_CMD_PSU_VOLTAGE:
            replyRequired = false;
            break;
#endif

#ifdef BOARD_ECU
        case COMM_CMD_START_RACE:
            if (packet->len == 1) {
                retval = Comm_StartRace(packet->payload[0]);
            }
            break;
        case COMM_CMD_ECU_DEBUG:
            if (packet->len == sizeof(comm_ecu_debug_t)) {
                retval = Comm_EcuDebug((comm_ecu_debug_t *) packet->payload);
            }
            break;
        case COMM_CMD_ECU_USER_IO:
            if (packet->len == sizeof(comm_ecu_user_io_t)) {
                retval = Comm_EcuUserIo((comm_ecu_user_io_t *) packet->payload);
            }
            break;
#endif

        default:
            retval = COMM_ERR_UNSUPPORTED_CMD;
            break;
    }

    if (replyRequired) {
        response.cmd.response = 1;
        if (response.len == 0 || retval != COMM_OK) {
            response.len = 1;
            response.payload[0] = retval;
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
