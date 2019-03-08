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
 * @file    can.h
 * @brief   Can driver
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __DRIVERS_CAN_H
#define __DRIVERS_CAN_H

#include <hal.h>
#include <types.h>

#define CAN_TIMEOUT_MS 1
#define CAN_TRANSMIT_RETRIES 5

/** Callback for received frame */
typedef bool (*cand_rx_cb_t)(const CANRxFrame *frame);

/**
 * Can filter
 *
 * Specifies can message IDs that should pass the filtering, id is the
 * id expected, mask specifies which bits of the id are important (1) and
 * which are ignored (0)
 */
typedef struct {
    uint32_t id;
    uint32_t mask;
} cand_filter_t;

/**
 * Send frame over CAN
 *
 * @param [in] frame        Frame to be send
 *
 * @return Successfulness of the operation
 */
extern bool Cand_SendFrame(const CANTxFrame *frame);

/**
 * Register callback for data received
 *
 * @param [in] cb   Callback to be registered
 */
extern void Cand_RegisterRxCb(cand_rx_cb_t cb);

/**
 * Initialize can driver
 *
 * @param [in] baudrate     CAN baudrate in b/s
 * @param [in] filters      CAN filters
 * @param [in] count        Amount of CAN filters
 */
extern void Cand_Init(uint32_t baudrate, const cand_filter_t *filters,
        uint8_t count);


#endif

/** @} */
