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
 * @file    log.c
 * @brief   Logging subsystem
 *
 * @addtogroup modules
 * @{
 */

#include <ch.h>
#include <hal.h>
#include <chprintf.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "utils/assert.h"
#include "modules/log.h"


#ifdef STM32F042x6
    #define LOG_BUFFER_LEN 4
#else
    #define LOG_BUFFER_LEN 16
#endif

typedef struct {
    log_severity_t sources[LOG_SOURCE_COUNT];
    bool external;      /** Interested also in logs from external modules */
    log_callback_t cb;
} log_subscription_t;

/** Minimum severity level to log */
static log_severity_t logi_max_severity = LOG_SEVERITY_NONE;

/** Array of log subscriptions */
static log_subscription_t logi_subscriptions[LOG_CALLBACKS_NUM];

/** Memory pool for messages */
static log_msg_t logi_messages[LOG_BUFFER_LEN];

/** Stack and stuff for thread */
THD_WORKING_AREA(logi_thread_area, 768);
MEMORYPOOL_DECL(logi_msg_pool, LOG_BUFFER_LEN, 4, 0);
static msg_t logi_letter[LOG_BUFFER_LEN];
MAILBOX_DECL(logi_mailbox, &logi_letter, LOG_BUFFER_LEN);

static void Logi_SendLog(log_msg_t *msg)
{
    int i;

    ASSERT_NOT(msg == NULL);

    for (i = 0; i < LOG_CALLBACKS_NUM; i++) {
        if (logi_subscriptions[i].cb == NULL) {
            continue;
        }
        if (logi_subscriptions[i].sources[msg->src] >= msg->severity) {
            if (msg->module == LOG_MODULE_MYSELF ||
                    logi_subscriptions[i].external) {
                logi_subscriptions[i].cb(msg);
            }
        }
    }
}

static THD_FUNCTION(Logi_Thread, arg)
{
    (void) arg;
    log_msg_t *msg;

    while (true) {
        chMBFetchTimeout(&logi_mailbox, (msg_t*)&msg, TIME_INFINITE);
        Logi_SendLog(msg);
        chPoolFree(&logi_msg_pool, msg);
    }
}

/**
 * Add message to log queue
 *
 * @param [in] module   Origin module of the message
 * @param [in] src      Source of the message
 * @param [in] severity Log message severity
 * @param [in] format   String format specifier
 * @param [in] ap       Variable arguments list
 * @param [in] bare_str Skip printf like formating, use bare string format
 */
static void Logi_AddEntry(log_module_t module, log_src_t src,
        log_severity_t severity, const char *format, va_list ap, bool bare_str)
{
    log_msg_t *msg;

    ASSERT_NOT(format == NULL);

    if (severity > logi_max_severity) {
        return;
    }

    msg = (log_msg_t *)chPoolAlloc(&logi_msg_pool);
    if (!msg) {
        /** out of memory */
        return;
    }

    if (bare_str) {
        strcpy(msg->msg, format);
    } else {
        chvsnprintf(msg->msg, LOG_MSG_LEN, format, ap);
    }
    msg->module = module;
    msg->src = src;
    msg->severity = severity;
    msg->time = chTimeI2S(chVTGetSystemTime());

    /** no need to check the return value, if it fails, it fails, log is lost */
    chMBPostTimeout(&logi_mailbox, (msg_t) msg, TIME_IMMEDIATE);
}

/**
 * Get highest priority level based on subscribtions
 *
 * @return highest severity found
 */
static log_severity_t Logi_GetHighestSeverity(void)
{
    int i, j;
    log_severity_t maxSeverity = LOG_SEVERITY_NONE;

    for (i = 0; i < LOG_CALLBACKS_NUM; i++) {
        if (logi_subscriptions[i].cb == NULL) {
            continue;
            }

            for (j = 0; j < LOG_SOURCE_COUNT; j++) {
            if (logi_subscriptions[i].sources[j] > maxSeverity) {
                maxSeverity = logi_subscriptions[i].sources[j];
            }
        }
    }

    return maxSeverity;
}

void Log_AddEntry(log_module_t module, log_src_t src, log_severity_t severity,
        const char *message)
{
    /* Va list is not used, there's no way to pass null, therefore setting
     * it to 0 and surpressing warning in code checker */
    va_list ap = {0};
    ASSERT_NOT(module >= LOG_MODULE_COUNT || src >= LOG_SOURCE_COUNT || severity
            >= LOG_SEVERITY_COUNT || message == NULL);

    Logi_AddEntry(module, src, severity, message, ap, true);
}

bool Log_Subscribe(log_callback_t cb, const log_severity_t severity[],
        bool external)
{
    int i;

    ASSERT_NOT(severity == NULL);

    for (i = 0; i < LOG_CALLBACKS_NUM; i++) {
        if (logi_subscriptions[i].cb == NULL) {
            logi_subscriptions[i].cb = cb;
            memcpy(logi_subscriptions[i].sources,
                    severity, LOG_SOURCE_COUNT*sizeof(severity[0]));
            logi_subscriptions[i].external = external;
            logi_max_severity = Logi_GetHighestSeverity();
            return true;
        }
    }
    return false;
}

bool Log_Unsubscribe(log_callback_t cb)
{
    int i;

    for (i = 0; i < LOG_CALLBACKS_NUM; i++) {
        if (logi_subscriptions[i].cb == cb) {
            logi_subscriptions[i].cb = NULL;
            logi_max_severity = Logi_GetHighestSeverity();
            return true;
        }
    }

    return false;
}

bool Log_UpdateSubscription(log_callback_t cb, const log_severity_t severity[])
{
    int i;

    ASSERT_NOT(severity == NULL);

    for (i = 0; i < LOG_CALLBACKS_NUM; i++) {
        if (logi_subscriptions[i].cb == cb) {
            memcpy(logi_subscriptions[i].sources,
                    severity, LOG_SOURCE_COUNT*sizeof(severity[0]));
            logi_max_severity = Logi_GetHighestSeverity();
            return true;
        }
    }
    return false;
}

bool Log_GetSubscription(log_callback_t cb, log_severity_t *severity)
{
    int i;
    ASSERT_NOT(severity == NULL);

    for (i = 0; i < LOG_CALLBACKS_NUM; i++) {
        if (logi_subscriptions[i].cb == cb) {
            memcpy(severity, logi_subscriptions[i].sources,
                    LOG_SOURCE_COUNT*sizeof(severity[0]));
            return true;
        }
    }
    return false;
}

void Log_Debug(log_src_t src, const char *format, ...)
{
    va_list ap;

    ASSERT_NOT(format == NULL);

    va_start(ap, format);
    Logi_AddEntry(LOG_MODULE_MYSELF, src, LOG_SEVERITY_DEBUG, format, ap, false);
    va_end(ap);
}

void Log_Info(log_src_t src, const char *format, ...)
{
    va_list ap;

    ASSERT_NOT(format == NULL);

    va_start(ap, format);
    Logi_AddEntry(LOG_MODULE_MYSELF, src, LOG_SEVERITY_INFO, format, ap, false);
    va_end(ap);
}

void Log_Warn(log_src_t src, const char *format, ...)
{
    va_list ap;

    ASSERT_NOT(format == NULL);

    va_start(ap, format);
    Logi_AddEntry(LOG_MODULE_MYSELF, src, LOG_SEVERITY_WARNING, format, ap, false);
    va_end(ap);
}

void Log_Error(log_src_t src, const char *format, ...)
{
    va_list ap;

    ASSERT_NOT(format == NULL);

    va_start(ap, format);
    Logi_AddEntry(LOG_MODULE_MYSELF, src, LOG_SEVERITY_ERROR, format, ap, false);
    va_end(ap);
}

const char *Log_GetSeverityStr(log_severity_t severity)
{
    static const char severityStr[LOG_SEVERITY_COUNT][8] = {
        "Unknown",
        "Error",
        "Warn",
        "Info",
        "Debug",
    };

    ASSERT_NOT(severity >= LOG_SEVERITY_COUNT);
    return severityStr[severity];
}

const char *Log_GetSourceStr(log_src_t src)
{
    static const char sourceStr[LOG_SOURCE_COUNT][8] = {
        "SYSTEM",
        "DRIVER",
        "MODULE",
        "COMM",
        "APP",
    };

    ASSERT_NOT(src >= LOG_SOURCE_COUNT);
    return sourceStr[src];
}

const char *Log_GetModuleStr(log_module_t module)
{
    static const char moduleStr[LOG_MODULE_COUNT][4] = {
        "ECU",
        "HMI",
        "PSU",
        "SDU",
        "RF",
    };

    ASSERT_NOT(module >= LOG_MODULE_COUNT);
    return moduleStr[module];
}

void Log_Init(void)
{
    int i;

    /* set data buffer to memory pool */
    for (i = 0; i < LOG_BUFFER_LEN; i++) {
        chPoolFree(&logi_msg_pool, &logi_messages[i]);
    }

    (void) chThdCreateStatic(logi_thread_area, sizeof(logi_thread_area),
                    NORMALPRIO, Logi_Thread, NULL);
}

/** @} */
