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

enum {
    COMM_CONFIG_TYPE_BOOL = 0x00,
    COMM_CONFIG_TYPE_UINT = 0x01,
};

typedef struct {
    uint8_t device_id;
    uint8_t state;
    uint16_t uptime_s;
    uint8_t core_temp_c;
    uint16_t core_voltage_mv;
} __attribute__((packed)) comm_system_status_t;

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
    uint8_t mode;
    uint8_t inputs;
    uint16_t speed_dms;
    uint16_t distance_m;
} __attribute__((packed)) comm_car_state;

typedef struct {
    uint16_t bat1_mv;
    uint16_t bat2_mv;
    uint16_t current_ma;
    uint8_t charge_pct;
} __attribute__((packed)) comm_battery_state;

typedef struct {
    uint16_t current5v_ma;
    uint16_t current12v_ma;
    uint16_t current24v_ma;
} __attribute__((packed)) comm_psu_current;

typedef struct {
    uint16_t voltage5v_ma;
    uint16_t voltage12v_ma;
    uint16_t voltage24v_ma;
} __attribute__((packed)) comm_psu_voltage;

/**
 * Process the received packet (requests only)
 *
 * @param [in] packet Packet to be processed
 * @param [in] send_iface   Callback to send response over
 *
 * @return true if suceeded
 */
extern bool Comm_HandlePacket(const comm_packet_t *packet,
        comm_send_cb_t send_iface);

#endif

/** @} */
