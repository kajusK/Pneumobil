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
 * @file    uart_link.h
 * @brief   Communication protocol, link layer of uart interface
 *
 * @details Reading and sending frames in format [0xff len data1 ... datan crc8]
 *          For incorrect crc [0xff 0x00 crc] is returned
 *
 * @addtogroup modules-comm
 * @{
 */

#ifndef __MODULES_COMM_UART_LINK_H
#define __MODULES_COMM_UART_LINK_H

#include <types.h>

#define COMM_UART_BAUDRATE  115200

/**
 * Create frame from payload and send to computer
 *
 * @param [in] payload      Payload to be send
 * @param [in] len          Length of the payload
 *
 * @return  Successfulness of the operation
 */
extern bool Comm_Uart_Send(const uint8_t *payload, uint8_t len);

/**
 * Initialize uart link layer and it's dependencies
 */
extern void Comm_Uart_Init(void);

#endif

/** @} */
