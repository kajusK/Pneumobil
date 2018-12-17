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

#include <types.h>
#include <version.h>

/** Communication node ids, used for CAN addressing */
typedef enum {
    COMM_NODE_DEBUG = 0x00,
    COMM_NODE_PSU = 0x01,
    COMM_NODE_HMI = 0x02,
    COMM_NODE_ECU = 0x03,
    COMM_NODE_SDU = 0x04,
    COMM_NODE_UNKNOWN = 0x05,
    COMM_NODE_BROADCAST = 0x0f,    /* For transmission only */
} comm_node_t;

/** Communication priority, used by CAN for prioritizing messages */
typedef enum {
    COMM_PRIORITY_CRITICAL = 0x00,
    COMM_PRIORITY_NORMAL = 0x01,
    COMM_PRIORITY_LOG = 0x02,
    COMM_PRIORITY_DEBUG = 0x03
} comm_priority_t;

#ifdef BOARD_ECU
    #define COMM_MY_ID COMM_NODE_ECU
#elif defined BOARD_HMI
    #define COMM_MY_ID COMM_NODE_HMI
#elif defined BOARD_PSU
    #define COMM_MY_ID COMM_NODE_PSU
#elif defined BOARD_SDU
    #define COMM_MY_ID COMM_NODE_SDU
#else
    #error "BOARD_NAME must be defined"
#endif

typedef bool (*comm_send_cb_t)(comm_node_t dest, comm_priority_t priority,
        const uint8_t *payload, uint8_t len);

extern bool Comm_HandlePayload(comm_node_t src, comm_node_t dest,
        const uint8_t *payload, uint8_t len, comm_send_cb_t send_frame);

#endif

/** @} */
