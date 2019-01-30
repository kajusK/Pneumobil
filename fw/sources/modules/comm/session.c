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
 * @file    session.c
 * @brief   Communication protocol, session layer
 *
 * Handling command-response type of communication
 *
 * @addtogroup modules-comm
 * @{
 */
#include <ch.h>
#include <string.h>

#include "utils/assert.h"
#include "modules/log.h"
#include "modules/comm/presentation.h"
#include "modules/comm/session.h"

/** Session type for command-response processing */
typedef struct {
    comm_cmd_t cmd;             /** Cmd expected in reply */
    comm_node_t dest;           /** Node id that data were send to */
    comm_packet_t *response;    /** Response memory to be filled */
    binary_semaphore_t bsem;    /** Binary semaphore to signal response received */
} comm_session_t;

/** Command-response session storage */
static comm_session_t commi_session;

/** Mutex for locking sending for commands with response required */
MUTEX_DECL(commi_session_mutex);

comm_error_t Comm_SendPayload(const comm_packet_t *packet,
        comm_packet_t *response, comm_send_cb_t link_iface)
{
    msg_t msg;
    uint8_t isError, len, errorCode;

    ASSERT_NOT(link_iface == NULL || packet == NULL);

    /* If response is required, sending must be locked using mutex */
    if (response != NULL) {
        chMtxLock(&commi_session_mutex);
    }

    if (link_iface(packet->node, packet->priority, (uint8_t *) &packet->cmd,
                packet->len + 1) != true) {
        return COMM_ERR_LINK;
    }

    if (response == NULL) {
        return COMM_OK;
    }

    commi_session.cmd = packet->cmd;
    commi_session.dest = packet->node;
    commi_session.response = response;
    chBSemReset(&commi_session.bsem, true);
    msg = chBSemWaitTimeout(&commi_session.bsem, TIME_MS2I(COMM_SESSION_TIMEOUT_MS));

    /* Store session content and return mutex */
    isError = commi_session.cmd.error;
    len = commi_session.response->len;
    errorCode = commi_session.response->payload[0];
    chMtxUnlock(&commi_session_mutex);

    if (msg != MSG_OK) {
        Log_Warn(LOG_SOURCE_COMM, "Response receive timeouted");
        return COMM_ERR_TIMEOUT;
    }

    if (isError) {
        if (len != 1) {
            Log_Warn(LOG_SOURCE_COMM, "Incorrect error payload length");
            return COMM_ERR_FORMAT;
        }
        return errorCode;
    }

    return COMM_OK;
}

bool Comm_HandlePayload(comm_node_t src, comm_node_t dest,
        const uint8_t *payload, uint8_t len, comm_send_cb_t send_iface)
{
    comm_cmd_t cmd;

    (void) dest;
    ASSERT_NOT(payload == NULL || send_iface == NULL);
    if (len == 0) {
        Log_Debug(LOG_SOURCE_COMM, "Received empty packet from %x", src);
        return false;
    }

    cmd.intval = payload[0];
    /* Not part of previously started communication */
    if (cmd.response == 0) {
        comm_packet_t packet;
        packet.cmd = cmd;
        memcpy(packet.payload, &payload[1], len - 1);
        packet.len = len - 1;
        packet.node = src;
        packet.priority = COMM_PRIORITY_NORMAL;
        return Comm_HandlePacket(dest, &packet, send_iface);
    }

    if (src != commi_session.dest) {
        Log_Warn(LOG_SOURCE_COMM, "Received response from %x, expected from %x",
                src, commi_session.dest);
        return false;
    }

    if (cmd.id != commi_session.cmd.id) {
        Log_Warn(LOG_SOURCE_COMM, "Received response to %02x, expected for %02x",
                cmd.id, commi_session.cmd.id);
        return false;
    }

    ASSERT_NOT(commi_session.response->payload == NULL);
    memcpy(commi_session.response->payload, payload + 1, len - 1);
    commi_session.response->len = len - 1;
    commi_session.response->cmd = cmd;
    commi_session.response->node = src;
    commi_session.response->priority = COMM_PRIORITY_NORMAL;
    chBSemSignal(&commi_session.bsem);
    return true;
}

void Comm_SessionInit(void)
{
    chBSemObjectInit(&commi_session.bsem, true);
}

/** @} */

