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

#include <stdio.h>
#include <string.h>

#include "utils/assert.h"
#include "modules/log.h"

#define LOG_BUFFER_LEN 16

typedef struct {
    log_severity_t sources[LOG_SOURCE_COUNT];
    log_callback_t cb;
} log_subscription_t;

/** Minimum severity level to log */
static log_severity_t logi_max_severity = LOG_SEVERITY_NONE;

/** Array of log subscriptions */
static log_subscription_t logi_subscriptions[LOG_CALLBACKS_NUM];

/** Memory pool for messages */
static log_msg_t logi_messages[LOG_BUFFER_LEN];

/** Stack and stuff for thread */
THD_WORKING_AREA(logi_thread_area, 128);
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
            logi_subscriptions[i].cb(msg);
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
 * Add message to log queue if enabled
 *
 * @param [in] src      source of the message
 * @param [in] severity log message severity
 * @param [in] format   format string specifier, same as for printf
 * @param [in] ad       initialized va_list of variable arguments
 */
static void Logi_AddEntry(log_src_t src, log_severity_t severity,
        const char *format, va_list ap)
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

    vsnprintf(msg->msg, LOG_MSG_LEN, format, ap);
    msg->src = src;
    msg->severity = severity;
    msg->time = TIME_I2S(chVTGetSystemTime());

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

bool Log_Subscribe(log_callback_t cb, const log_severity_t severity[])
{
    int i;

    ASSERT_NOT(severity == NULL);

    for (i = 0; i < LOG_CALLBACKS_NUM; i++) {
        if (logi_subscriptions[i].cb == NULL) {
            logi_subscriptions[i].cb = cb;
            memcpy(logi_subscriptions[i].sources,
                    severity, LOG_SOURCE_COUNT*sizeof(severity[0]));
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

void Log_Debug(log_src_t src, const char *format, ...)
{
    va_list ap;

    ASSERT_NOT(format == NULL);

    va_start(ap, format);
    Logi_AddEntry(src, LOG_SEVERITY_DEBUG, format, ap);
    va_end(ap);
}

void Log_Info(log_src_t src, const char *format, ...)
{
    va_list ap;

    ASSERT_NOT(format == NULL);

    va_start(ap, format);
    Logi_AddEntry(src, LOG_SEVERITY_INFO, format, ap);
    va_end(ap);
}

void Log_Warn(log_src_t src, const char *format, ...)
{
    va_list ap;

    ASSERT_NOT(format == NULL);

    va_start(ap, format);
    Logi_AddEntry(src, LOG_SEVERITY_WARNING, format, ap);
    va_end(ap);
}

void Log_Error(log_src_t src, const char *format, ...)
{
    va_list ap;

    ASSERT_NOT(format == NULL);

    va_start(ap, format);
    Logi_AddEntry(src, LOG_SEVERITY_ERROR, format, ap);
    va_end(ap);
}

void Log_Init(void)
{
    int i;

    (void) chThdCreateStatic(logi_thread_area, sizeof(logi_thread_area),
                    NORMALPRIO, Logi_Thread, NULL);

    /** set data buffer to memory pool */
    for (i = 0; i < LOG_BUFFER_LEN; i++) {
        chPoolFree(&logi_msg_pool, &logi_messages[i]);
    }
}

/** @} */
