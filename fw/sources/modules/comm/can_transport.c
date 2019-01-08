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
 * @file    can_transport.c
 * @brief   Communication protocol, transport layer of can interface
 *
 * @addtogroup modules-comm
 * @{
 */

#include <string.h>

#include "utils/assert.h"
#include "modules/config.h"
#include "modules/log.h"
#include "utils/crc.h"
#include "drivers/can.h"
#include "modules/comm/session.h"
#include "modules/comm/can_transport.h"

/** Max length of the can data frame in bytes */
#define CAN_DATA_LEN 8

/** Structure for holding multipart frames (mailboxes) being received */
typedef struct {
    uint16_t len;       /** Amount of bytes expected */
    uint16_t received;  /** Amount of bytes received */
    bool used;          /** This slot is being used for saving right now */
    uint8_t src_id;     /** ID of the source node */
    uint8_t seq;        /** Expected sequence number of the frame */
    uint8_t payload[COMM_MAX_PAYLOAD_LEN];
} comm_can_mbox_t;

/** Can ID format */
typedef union {
    struct {
        uint16_t priority : 2;      /** Priority of the message */
        uint16_t src : 4;           /** Identifier of source node */
        uint16_t dest : 4;          /** Identifier of destination node */
        uint16_t multipart : 1;     /** multiple part message will follow */
    };
    uint16_t intval;
} comm_can_id_t;

/** Storage area for multipart messages */
static comm_can_mbox_t cani_rx_mbox[COMM_CAN_MBOX_SLOTS];

/**
 * Find mailbox slot to use for storing multipart messafe from given source
 *
 * @param [in] src_id       Id of the source node
 * @param [in] first        Looking for slot for first packet of multipart message
 * @return  slot id or -1 if no usable slot found
 */
static int8_t Commi_CanFindMultipartSlot(uint8_t src_id, bool first)
{
    /* Try to find used slot with same sourse */
    for (int i = 0; i < COMM_CAN_MBOX_SLOTS; i++) {
        if (cani_rx_mbox[i].src_id == src_id && cani_rx_mbox[i].used) {
            if (first) {
                Log_Warn(LOG_SOURCE_COMM,
                        "CAN overwritting previous mailbox for src %x", src_id);
            }
            return i;
        }
    }

    if (!first) {
        Log_Debug(LOG_SOURCE_COMM,
                "Ignoring unexpected multipart CAN from src %x", src_id);
        return -1;
    }

    /* find first unused slot */
    for (int i = 0; i < COMM_CAN_MBOX_SLOTS; i++) {
        if (cani_rx_mbox[i].used == false) {
            return i;
        }
    }

    Log_Error(LOG_SOURCE_COMM,
            "No available CAN mailbox, ignoring msg from %x", src_id);
    return -1;
}

/**
 * Process a multipart CAN message
 *
 * @param [in] frame    Received CAN frame
 * @return true if processing suceeded
 */
static bool Commi_CanProcessMultipart(const CANRxFrame *frame)
{
    comm_can_id_t id;
    int8_t mboxId;
    uint8_t offset = 0;
    uint16_t bytesToCopy = 0;
    uint16_t remaining = 0;
    bool lastFrame = false;
    uint8_t crc;

    ASSERT_NOT(frame == NULL);

    id.intval = frame->SID;

    if (frame->DLC < 2) {
        Log_Warn(LOG_SOURCE_COMM,
                "CAN multipart message with no payload from %x", id.src);
        return false;
    }

    mboxId = Commi_CanFindMultipartSlot(id.src, frame->data8[0] == 0x00);
    if (mboxId < 0 || mboxId >= COMM_CAN_MBOX_SLOTS) {
        return false;
    }

    /* new message */
    if (frame->data8[0] == 0x00) {
        if (frame->data8[1] > COMM_MAX_PAYLOAD_LEN) {
            Log_Warn(LOG_SOURCE_COMM,
                "CAN multipart from %x too long (%d bytes)", id.src, frame->data8[1]);
            return false;
        }

        cani_rx_mbox[mboxId].src_id = id.src;
        cani_rx_mbox[mboxId].len = frame->data8[1];
        cani_rx_mbox[mboxId].used = true;
        cani_rx_mbox[mboxId].seq = 1;
        cani_rx_mbox[mboxId].received = 0;
        offset = 2;
    } else {
        if (cani_rx_mbox[mboxId].seq != frame->data8[0]) {
            Log_Warn(LOG_SOURCE_COMM,
                    "CAN multipart id doesn't match, src %x, got %d, expected %d",
                    id.src, frame->data8[0], cani_rx_mbox[mboxId].seq);
            cani_rx_mbox[mboxId].used = false;
            return false;
        }
        cani_rx_mbox[mboxId].seq++;

        offset = 1;
    }

    remaining = cani_rx_mbox[mboxId].len - cani_rx_mbox[mboxId].received;
    if (remaining >= frame->DLC - offset) {
        bytesToCopy = frame->DLC - offset;
    } else if (remaining == frame->DLC - offset - 1) {
        /* last byte in buffer is crc */
        bytesToCopy = frame->DLC - offset - 1;
        lastFrame = true;
    } else {
        Log_Warn(LOG_SOURCE_COMM, "CAN multipart msg is too long, cropping");
        bytesToCopy = remaining;
        lastFrame = true;
    }

    memcpy(cani_rx_mbox[mboxId].payload + cani_rx_mbox[mboxId].received,
            &frame->data8[offset], bytesToCopy);
    cani_rx_mbox[mboxId].received += bytesToCopy;

    if (!lastFrame) {
        return true;
    }

    cani_rx_mbox[mboxId].used = false;

    crc = CRC8(cani_rx_mbox[mboxId].payload, cani_rx_mbox[mboxId].len);
    if (crc != frame->data8[offset+bytesToCopy]) {
        Log_Debug(LOG_SOURCE_COMM,
                "CAN crc from %x failed, got %x, expected %x",
                id.src, crc, frame->data8[offset+bytesToCopy]);
        return false;
    }

    return Comm_HandlePayload(id.src, id.dest, cani_rx_mbox[mboxId].payload,
            cani_rx_mbox[mboxId].len, Comm_CanSend);
}

/**
 * Process frame received over CAN
 *
 * @param [in] frame    Received CAN frame
 * @return      True if processing suceeded
 */
static bool Commi_CanProcessFrame(const CANRxFrame *frame)
{
    comm_can_id_t id;

    ASSERT_NOT(frame == NULL);

    /* extended identifier, must be false, we are not using those */
    if (frame->IDE) {
        Log_Warn(LOG_SOURCE_COMM, "CAN received extended identifier");
        return false;
    }
    /* remote request are not used */
    if (frame->RTR) {
        Log_Warn(LOG_SOURCE_COMM, "CAN received remote request");
        return false;
    }

    /** No need to filter ids, this is done in hardware based on can filters */
    id.intval = frame->SID;
    if (!id.multipart) {
        return Comm_HandlePayload(id.src, id.dest, frame->data8, frame->DLC,
                Comm_CanSend);
    }

    return Commi_CanProcessMultipart(frame);
}

bool Comm_CanSend(comm_node_t dest, comm_priority_t priority,
        const uint8_t *payload, uint8_t len)
{
    uint16_t sent;
    uint8_t seq = 0;
    uint8_t offset;
    uint8_t crc;
    bool crcSent;
    CANTxFrame frame;
    comm_can_id_t id;

    ASSERT_NOT(payload == NULL);
    if (len > COMM_MAX_PAYLOAD_LEN) {
        Log_Debug(LOG_SOURCE_COMM, "Trying to send too long CAN frame (%d)", len);
        return false;
    }
    if (len == 0) {
        Log_Debug(LOG_SOURCE_COMM, "Trying to send empty CAN frame (%d)", len);
        return false;

    }

    frame.RTR = 0;
    frame.IDE = 0;
    id.src = COMM_MY_ID;
    id.dest = dest;
    id.priority = priority;
    frame.SID = id.intval;

    /* Small frame */
    if (len <= 8) {
        frame.DLC = len;
        memcpy(frame.data8, payload, len);
        return Cand_SendFrame(&frame);
    }

    /* multipart message */
    seq = 0;
    sent = 0;
    offset = 2;
    frame.data8[1] = len;
    crc = CRC8(payload, len);
    crcSent = false;

    while (sent <= len && crcSent != true) {
        uint8_t remaining = len - sent;
        uint8_t sublen;
        if (remaining >= (CAN_DATA_LEN - offset)) {
            sublen = CAN_DATA_LEN - offset;
            frame.DLC = sublen + offset;
        } else {
            sublen = remaining;
            frame.data8[sublen+offset] = crc;
            crcSent = true;
            frame.DLC = sublen + offset + 1;
        }
        frame.data8[0] = seq;
        memcpy(&frame.data8[offset], payload+sent, sublen);

        if (!Cand_SendFrame(&frame)) {
            Log_Debug(LOG_SOURCE_COMM, "Failed to send CAN frame");
            return false;
        }
        sent += sublen;
        seq++;
        offset = 1;
    }

    return true;
}

void Comm_CanInit(void)
{
    cand_filter_t filters;
    comm_can_id_t id, mask;

    id.dest = COMM_MY_ID;
    mask.dest = COMM_NODE_BROADCAST;
    filters.id = id.intval;
    filters.mask = mask.intval;

    if (Config_GetBool(CONFIG_BOOL_CAN_PROMISCUOUS)) {
        Log_Info(LOG_SOURCE_COMM, "Running CAN inpromiscuous mode");
        mask.intval = 0;
    }

    Cand_Init(COMM_CAN_BAUDRATE, &filters, 1);
    Cand_RegisterRxCb(Commi_CanProcessFrame);
}

/** @} */
