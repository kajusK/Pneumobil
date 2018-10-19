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
 * @file    uart.h
 * @brief   Uart driver
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __DRIVERS_UART_H
#define __DRIVERS_UART_H

#include <types.h>

typedef void (*uartd_rx_cb_t)(uint8_t c);

/**
 * Register uart data received callback
 *
 * @param [in] cb       Callback for rx data
 */
extern void Uartd_RegisterRxCb(uartd_rx_cb_t cb);

/**
 * Send byte over uart, blocking
 *
 * @param [in] byte     Byte to send
 */
extern void Uartd_SendByte(uint8_t byte);

/**
 * Initialize uart driver
 *
 * @param [in] baudrate Desired uart baudrate
 */
extern void Uartd_Init(uint32_t baudrate);

#endif

/** @} */
