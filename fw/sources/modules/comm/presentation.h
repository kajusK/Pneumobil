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
 * @file    presentation.h
 * @brief   Communication protocol, presentation layer
 *
 * @addtogroup modules-comm
 * @{
 */

#ifndef __MODULES_COMM_PRESENTATION_H
#define __MODULES_COMM_PRESENTATION_H

#include "modules/log.h"
#include "modules/comm/comm.h"
#include "modules/comm/session.h"
#include "modules/comm/can_transport.h"
#include "modules/comm/uart_transport.h"

/** Interfaces to send data over */
#define IFACE_CAN Comm_CanSend
#define IFACE_UART Comm_UartSend

enum {
    COMM_CONFIG_TYPE_BOOL = 0x00,
    COMM_CONFIG_TYPE_UINT = 0x01,
};

enum {
    COMM_LOG_IFACE_CAN = 0x00,
    COMM_LOG_IFACE_UART = 0x01,
};

/** Below are the packet formats, more in doc/specs/protocols.ods sheet commands */

typedef struct {
    uint8_t device_id;
    uint8_t state;
    uint16_t uptime_s;
    uint8_t core_temp_c;
    uint16_t core_voltage_mv;
} __attribute__((packed)) comm_sys_status_t;

typedef struct {
    uint8_t interface;
    uint8_t severity[LOG_SOURCE_COUNT];
} __attribute__((packed)) comm_log_mask_t;

typedef struct {
    uint8_t id;
    uint8_t type;
    union {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
    };
} __attribute__((packed)) comm_config_item_t;

typedef struct {
    uint8_t source;
    uint8_t severity;
    uint8_t message[LOG_MSG_LEN];
} __attribute__((packed)) comm_log_msg_t;

typedef struct {
    uint16_t speed_dms;
    uint16_t speed_avg_dms;
    uint16_t distance_m;
    uint8_t mode;
} __attribute__((packed)) comm_car_state_t;

typedef struct {
    uint8_t valves;
    uint8_t endstops;
    uint8_t inputs;
    uint8_t gear;
} __attribute__((packed)) comm_car_io_t;

typedef struct {
    uint16_t press1_kpa;
    uint16_t press2_kpa;
    uint16_t press3_kpa;
    uint8_t piston_pct;
} __attribute__((packed)) comm_pneu_state_t;

typedef struct {
    uint8_t valves;
    uint8_t outputs;
} __attribute__((packed)) comm_ecu_debug_t;

typedef struct {
    uint8_t horn;
    uint8_t brake;
    uint8_t out1;
    uint8_t out2;
} __attribute__((packed)) comm_ecu_user_io_t;

typedef struct {
    uint16_t bat1_mv;
    uint16_t bat2_mv;
    uint16_t current_ma;
    uint8_t charge_pct;
} __attribute__((packed)) comm_battery_state_t;

typedef struct {
    uint16_t current5v_ma;
    uint16_t current12v_ma;
    uint16_t current24v_ma;
} __attribute__((packed)) comm_psu_current_t;

typedef struct {
    uint16_t voltage5v_mv;
    uint16_t voltage12v_mv;
    uint16_t voltage24v_mv;
} __attribute__((packed)) comm_psu_voltage_t;

/**
 * Send log message over specified interface
 *
 * @param [in] msg      Log message
 * @param [in] iface    Interface to send message over
 */
extern bool Comm_SendLog(comm_send_cb_t iface, const log_msg_t *msg);

/**
 * Send broadcast packet over default interface, don't expect response
 *
 * @param [in] cmd      Command to send
 * @param [in] data     Pointer to data buffer
 * @param [in] len      Lenght of the data buffer
 *
 * @return True if suceeded
 */
extern bool Comm_SendPacket(comm_cmd_id_t cmd, const uint8_t *data, uint8_t len);

/**
 * Send unicast packet over default interface, wait for the response
 *
 * @param [in] target   Destination node
 * @param [in] cmd      Command to send
 * @param [in] data     Pointer to data buffer, or NULL for empty packet
 * @param [in] len      Lenght of the data buffer (must be 0 for NULL data)
 * @param [out] response    Buffer to copy response to or NULL if only erorr
 *                      returned should be checked
 * @param [in] rsp_len  Length of the response buffer, if message is of
 *                      different size, false is returned
 *
 * @return True if suceeded (node responded with OK or message)
 */
extern bool Comm_SendPacketCmd(comm_node_t target, comm_cmd_id_t cmd,
        const uint8_t *data, uint8_t len, uint8_t *response, uint8_t rsp_len);

/**
 * Process the received packet (requests only)
 *
 * @param [in] dest         Destination node (should be our id or broadcast
 * @param [in] packet       Packet to be processed
 * @param [in] send_iface   Callback to send response over
 *
 * @return true if suceeded
 */
extern bool Comm_HandlePacket(comm_node_t dest, const comm_packet_t *packet,
        comm_send_cb_t send_iface);

#endif

/** @} */
