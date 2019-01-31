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
 * @file    hmi/logger.c
 * @brief   Logging to the SD card
 *
 * Handles logging the syslog (log module output) to sdcard and generating
 * a race log
 *
 * @addtogroup app-hmi
 */

#ifndef __MODULES_LOGGER_H
#define __MODULES_LOGGER_H

#include <types.h>
#include <modules/log.h>

/**
 * Create new race log file
 */
extern bool Logger_NewRaceLogFile(void);

/**
 * Generates a new row to the race log
 *
 * Required data are fetched from other modules
 */
extern void Logger_AddRaceLogEntry(void);

/**
 * Add log from external source (received over can,...) to syslog
 *
 * @param [in] src      Source mode of the log message
 * @param [in] severity Severity of the message
 * @param [in] msg      Message to be logged
 * @param [in] len      Message len
 */
extern void Logger_AddSyslogExternal(log_src_t src, log_severity_t severity,
        const char *msg, uint8_t len);

/**
 * Initialize logger module
 */
extern void Logger_Init(void);

#endif

/** @} */
