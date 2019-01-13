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
 * @file    session.h
 * @brief   Communication protocol, session layer
 *
 * Handling command-response type of communication
 *
 * @addtogroup modules-comm
 * @{
 */

#ifndef __MODULES_COMM_SESSION_H
#define __MODULES_COMM_SESSION_H

#include <types.h>
#include "modules/comm/comm.h"

/** Timeout in ms for waiting for response to be received */
#define COMM_SESSION_TIMEOUT_MS 10

/** Command structure */
typedef union {
    struct {
        uint8_t response : 1;       /** Frame is response to request */
        uint8_t error : 1;          /** Error during request processing */
        comm_cmd_id_t id : 6;       /** ID of the command */
    };
    uint8_t intval;
} comm_cmd_t;

/** Packet received/sent over the interface */
typedef struct {
    comm_node_t node;               /** other node address */
    comm_priority_t priority;       /** Priority of the message */
    uint8_t len;                    /** Length of the payload */
    /* Must be in this order, address of cmd must be followed by payload */
    comm_cmd_t cmd;                 /** Command data */
    uint8_t payload[COMM_MAX_PAYLOAD_LEN-1];   /** Payload of the packet */
} comm_packet_t;

/**
 * Callback for sending response to requests
 *
 * @param [in] dest     Destination node ID
 * @param [in] priority Priority of the data frame (used over CAN)
 * @param [in] payload  Payload to be send
 * @param [in] len      Length of the payload
 *
 * @return True if suceeded
 */
typedef bool (*comm_send_cb_t)(comm_node_t dest, comm_priority_t priority,
        const uint8_t *payload, uint8_t len);

/**
 * Send data over specified interface, wait for reply if required
 *
 * @param [in] packet       Packet to be sent
 * @param [out] response    Memory area to store response to, NULL if not required
 * @param [in] priority     Message priority
 * @param [in] link_iface   Function to send data to
 */
extern comm_error_t Comm_SendPayload(const comm_packet_t *packet,
        comm_packet_t *response, comm_send_cb_t link_iface);

/**
 * Process the received data frame
 *
 * @param [in] src      ID of the sending node
 * @param [in] dest     ID of the receiving node
 * @param [in] payload  Data received
 * @param [in] len      Length of the payload
 * @param [in] send_iface   Pointer to function to be called when sending response
 *
 * @return  True if processed ok
 */
extern bool Comm_HandlePayload(comm_node_t src, comm_node_t dest,
        const uint8_t *payload, uint8_t len, comm_send_cb_t send_iface);

/**
 * Initialize session layer
 */
extern void Comm_SessionInit(void);

#endif

/** @} */
