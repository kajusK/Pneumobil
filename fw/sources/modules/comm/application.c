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
 * @file    application.c
 * @brief   Communication protocol, application layer
 *
 * @addtogroup modules-comm
 * @{
 */

#include <string.h>

#include "utils/assert.h"

#include "modules/log.h"
#include "modules/comm/comm.h"
#include "modules/comm/can_transport.h"
#include "modules/comm/uart_transport.h"
#include "modules/comm/presentation.h"
#include "modules/comm/session.h"

#include "modules/comm/application.h"

#define CAN_IFACE Comm_CanSend
#define UART_IFACE Comm_UartSend

static void Commi_SendLogCan(const log_msg_t *log)
{
    ASSERT_NOT(log == NULL);
    Comm_SendLog(log, CAN_IFACE);
}

static void Commi_SendLogUart(const log_msg_t *log)
{
    ASSERT_NOT(log == NULL);
    Comm_SendLog(log, UART_IFACE);
}

void Comm_Init(void)
{
    log_severity_t severity[LOG_SOURCE_COUNT];

    Comm_SessionInit();
    Comm_CanInit();
    Comm_UartInit();

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_WARNING;
    }
    severity[LOG_SOURCE_SYSTEM] = LOG_SEVERITY_INFO;
    severity[LOG_SOURCE_ECU] = LOG_SEVERITY_INFO;
    Log_Subscribe(Commi_SendLogCan, severity);

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_INFO;
    }
    Log_Subscribe(Commi_SendLogUart, severity);
}

/** @} */
