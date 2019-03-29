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
 * @file    uart.c
 * @brief   Uart driver
 *
 * @addtogroup drivers
 * @{
 */

#include <ch.h>
#include <hal.h>

#include "drivers/uart.h"

#define UARTD_THREAD_PRIO   NORMALPRIO

/** Usard serial driver */
#define SD JOIN(SD, USART_DEBUG_TXD)

/** callback for uart byte received */
static uartd_rx_cb_t uartdi_rx_cb;

/** Stack and stuff for thread */
THD_WORKING_AREA(uartdi_thread_area, 512);

static THD_FUNCTION(Uartd_Thread, arg)
{
    (void) arg;
    char c;

    while (true) {
        c = sdGet(&SD);
        if (uartdi_rx_cb != NULL) {
            uartdi_rx_cb(c);
        }
    }
}

void Uartd_RegisterRxCb(uartd_rx_cb_t cb)
{
    uartdi_rx_cb = cb;
}

void Uartd_SendByte(uint8_t byte)
{
    sdPut(&SD, byte);
}

void Uartd_Init(uint32_t baudrate)
{
    static SerialConfig sdConfig;

    sdConfig.speed = baudrate;
    sdConfig.cr1 = 0;
    sdConfig.cr2 = USART_CR2_STOP1_BITS;
    sdConfig.cr3 = 0;

    sdStart(&SD, &sdConfig);

    (void) chThdCreateStatic(uartdi_thread_area, sizeof(uartdi_thread_area),
                    UARTD_THREAD_PRIO, Uartd_Thread, NULL);
}

/** @} */

