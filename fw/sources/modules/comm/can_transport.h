/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

    This file is part of BUT pneumobil ECU.

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
 * @file    can_transport.h
 * @brief   Communication protocol, transport layer of can interface
 *
 * @addtogroup modules-comm
 * @{
 */

#ifndef __MODULES_COMM_CAN_TRANSPORT_H
#define __MODULES_COMM_CAN_TRANSPORT_H

#include <types.h>
#include "modules/comm/comm.h"

/** Amount of slots for multipart messages beying received simultaneously */
#define COMM_CAN_MBOX_SLOTS 4

/**
 * Can baudrate to be used
 * STM32_PCLK/baudrate/16 should be natural number
 */
#define COMM_CAN_BAUDRATE 500000

/**
 * Send message over CAN
 *
 * @param [in] dest         Destination node
 * @param [in] priority     Message priority
 * @param [in] payload      Data buffer
 * @param [in] len          Length of the buffer
 *
 * @return True if suceeded
 */
extern bool Comm_CanSend(comm_node_t dest, comm_priority_t priority,
        const uint8_t *payload, uint8_t len);

extern void Comm_CanInit(void);

#endif
/** @} */
