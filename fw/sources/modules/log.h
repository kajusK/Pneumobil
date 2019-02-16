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
 * @file    log.h
 * @brief   Logging subsystem
 *
 * @addtogroup modules
 * @{
 */

#ifndef __MODULES_LOG_H
#define __MODULES_LOG_H

#include <types.h>
#include "version.h"

/*
 * Maximum length of a log message
 */
#define LOG_MSG_LEN     64

/**
 * Amount of callback allowed
 */
#define LOG_CALLBACKS_NUM   3

/**
 * Log severity
 */
typedef enum {
    LOG_SEVERITY_NONE,
    LOG_SEVERITY_ERROR,
    LOG_SEVERITY_WARNING,
    LOG_SEVERITY_INFO,
    LOG_SEVERITY_DEBUG,
    LOG_SEVERITY_COUNT,
} log_severity_t;

/**
 * Log message sources to report
 */
typedef enum {
    LOG_SOURCE_SYSTEM,
    LOG_SOURCE_DRIVER,
    LOG_SOURCE_MODULE,
    LOG_SOURCE_COMM,
    LOG_SOURCE_ECU,
    LOG_SOURCE_HMI,
    LOG_SOURCE_POWER,
    LOG_SOURCE_COUNT,
} log_src_t;

/**
 * Origin module of the message
 */
typedef enum {
    LOG_MODULE_ECU,
    LOG_MODULE_HMI,
    LOG_MODULE_PSU,
    LOG_MODULE_SDU,
    LOG_MODULE_COUNT,
} log_module_t;

#ifdef BOARD_HMI
    #define LOG_MODULE_MYSELF LOG_MODULE_HMI
#elif defined(BOARD_ECU)
    #define LOG_MODULE_MYSELF LOG_MODULE_ECU
#elif defined(BOARD_PSU)
    #define LOG_MODULE_MYSELF LOG_MODULE_PSU
#elif defined(BOARD_SDU)
    #define LOG_MODULE_MYSELF LOG_MODULE_SDU
#else
    #error Undefined board
#endif

/**
 * Logger entry
 */
typedef struct {
    uint32_t time;          /** Seconds since boot */
    log_module_t module;
    log_severity_t severity;
    log_src_t src;
    char msg[LOG_MSG_LEN];
} log_msg_t;

/** Callback type for log sending */
typedef void (*log_callback_t)(const log_msg_t *msg);

/**
 * Add message to log queue
 *
 * Specific function should be used (Log_Error,...) instead for normal logging,
 * this function is intended for logging logs received from other modules
 *
 * @param [in] module   Origin module of the message
 * @param [in] src      Source of the message
 * @param [in] severity Log message severity
 * @param [in] message  Message string to be logged
 */
extern void Log_AddEntry(log_module_t module, log_src_t src,
        log_severity_t severity, const char *message);

/**
 * Subscribe to logs
 *
 * @param [in] cb       Callback to be called upon log reception
 * @param [in] severity Array of severities for each log_src_t item
 * @param [in] external Interested in logs from other boards than this one
 *
 * @return  true if succeeded
 */
extern bool Log_Subscribe(log_callback_t cb, const log_severity_t severity[],
        bool external);

/**
 * Unsubscribe callback from logs
 *
 * @param [in] cb       Callback that should be unsubscribed
 *
 * @return  true if succeeded
 */
extern bool Log_Unsubscribe(log_callback_t cb);

/**
 * Subscribe to logs
 *
 * @param [in] cb       Callback to be updated
 * @param [in] severity Array of severities for each log_src_t item
 *
 * @return  true if succeeded
 */
extern bool Log_UpdateSubscription(log_callback_t cb,
        const log_severity_t severity[]);

/**
 * Get subscribed severities
 *
 * @param [in] cb       Callback for which severities will be returned
 * @param [out] severity Memory LOG_SOURCE_COUNT long for storage of severities
 *
 * @return  true if succeeded
 */
extern bool Log_GetSubscription(log_callback_t cb, log_severity_t *severity);

/**
 * Log debug message
 *
 * @param [in] src      source of the message
 * @param [in] format   format string specifier, same as for printf
 * @param [in] ...      variable argument list based on format specifier
 */
extern void Log_Debug(log_src_t src, const char *format, ...);

/**
 * Log informational message
 *
 * @param [in] src      source of the message
 * @param [in] format   format string specifier, same as for printf
 * @param [in] ...      variable argument list based on format specifier
 */
extern void Log_Info(log_src_t src, const char *format, ...);

/**
 * Log warning message
 *
 * @param [in] src      source of the message
 * @param [in] format   format string specifier, same as for printf
 * @param [in] ...      variable argument list based on format specifier
 */
extern void Log_Warn(log_src_t src, const char *format, ...);

/**
 * Log error message
 *
 * @param [in] src      source of the message
 * @param [in] format   format string specifier, same as for printf
 * @param [in] ...      variable argument list based on format specifier
 */
extern void Log_Error(log_src_t src, const char *format, ...);

/**
 * Get name of the given severity in string
 *
 * @param [in] severity     Severity to get name for
 * @return Pointer to constant string
 */
extern const char *Log_GetSeverityStr(log_severity_t severity);

/**
 * Get name of the given log source in string
 *
 * @param [in] src     Log source to get name for
 * @return Pointer to constant string
 */
extern const char *Log_GetSourceStr(log_src_t src);

/**
 * Get name of the module that sent this message
 *
 * @param [in] module       Module id
 * @return Pointer to module string
 */
extern const char *Log_GetModuleStr(log_module_t module);

/**
 * Initialize logging module
 */
extern void Log_Init(void);

#endif

/** @} */
