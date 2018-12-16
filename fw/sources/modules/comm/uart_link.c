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
 * @file    uart_link.c
 * @brief   Communication protocol, link layer of uart interface
 *
 * @addtogroup modules-comm
 * @{
 */

#include <ch.h>

#include "utils/assert.h"
#include "modules/log.h"
#include "utils/crc.h"
#include "drivers/uart.h"
#include "modules/comm/comm.h"
#include "modules/comm/presentation.h"
#include "modules/comm/uart_link.h"

/** Byte value defining start of the frame */
#define COMM_UART_FRAME_START 0xff

/** Maximum delay between bytes to restart reception */
#define COMM_UART_MAX_DELAY_MS 2

/**
 * Structure for holding frame being received
 */
struct frame_s {
    uint16_t len;
    uint8_t payload[COMM_MAX_PAYLOAD_LEN];
};

/**
 *  Current reception phase
 */
enum comm_uart_phase_e {
    UART_PHASE_START,
    UART_PHASE_LEN,
    UART_PHASE_PAYLOAD,
    UART_PHASE_CRC,
};

MUTEX_DECL(uarti_tx_mutex);

/**
 * Process received byte and call frame handler if whole frame received
 *
 * @param [in] c    Byte received
 */
static void Commi_UartProcessByte(uint8_t c)
{
    static enum comm_uart_phase_e phase = UART_PHASE_START;
    static struct frame_s frame;
    static uint8_t bytesReceived = 0;
    static systime_t lastCalled = 0;
    static uint8_t crc = 0;

    /** Reset reception if delay between characters is too long */
    if (TIME_I2MS(chVTTimeElapsedSinceX(lastCalled)) > COMM_UART_MAX_DELAY_MS) {
        if (phase != UART_PHASE_START) {
            Log_Warn(LOG_SOURCE_COMM, "Uart rx timeouted");
            phase = UART_PHASE_START;
            bytesReceived = 0;
        }
    }
    lastCalled = chVTGetSystemTime();

    switch (phase) {
        case UART_PHASE_START:
            if (c == COMM_UART_FRAME_START) {
                crc = CRC8_Add(c, CRC8_INITIAL_VALUE);
                phase = UART_PHASE_LEN;
            } else {
                phase = UART_PHASE_START;
            }
            break;

        case UART_PHASE_LEN:
            crc = CRC8_Add(c, crc);
            frame.len = c;
            bytesReceived = 0;
            if (frame.len > COMM_MAX_PAYLOAD_LEN) {
                phase = UART_PHASE_START;
                Log_Warn(LOG_SOURCE_COMM, "Uart frame too long");
            }
            phase = UART_PHASE_PAYLOAD;
            break;

        case UART_PHASE_PAYLOAD:
            crc = CRC8_Add(c, crc);
            frame.payload[bytesReceived] = c;
            bytesReceived++;
            if (bytesReceived == frame.len) {
                phase = UART_PHASE_CRC;
            } else {
                phase = UART_PHASE_PAYLOAD;
            }
            break;

        case UART_PHASE_CRC:
            if (crc == c) {
                Comm_HandlePayload(COMM_NODE_DEBUG, COMM_MY_ID, frame.payload,
                        frame.len, Comm_UartSend);
                phase = UART_PHASE_START;
                Log_Debug(LOG_SOURCE_COMM, "Uart: got %d bytes", frame.len);
            } else {
                Log_Warn(LOG_SOURCE_COMM, "Uart got bad CRC");
            }
            break;

        default:
            phase = UART_PHASE_START;
            break;
    }
}

bool Comm_UartSend(comm_node_t dest, comm_priority_t priority,
        const uint8_t *payload, uint8_t len)
{
    int i;
    uint8_t crc;

    ASSERT_NOT(payload == NULL);
    (void) priority;
    (void) dest;

    chMtxLock(&uarti_tx_mutex);
    Uartd_SendByte(COMM_UART_FRAME_START);
    crc = CRC8_Add(COMM_UART_FRAME_START, CRC8_INITIAL_VALUE);
    Uartd_SendByte(len);
    crc = CRC8_Add(len, crc);

    for (i = 0; i < len; i++) {
        Uartd_SendByte(payload[i]);
        crc = CRC8_Add(payload[i], crc);
    }
    Uartd_SendByte(crc);
    chMtxUnlock(&uarti_tx_mutex);

    return true;
}

void Comm_UartInit(void)
{
    Uartd_Init(COMM_UART_BAUDRATE);
    Uartd_RegisterRxCb(Commi_UartProcessByte);
}

/** @} */
