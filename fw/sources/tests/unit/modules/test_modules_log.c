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
 * @file    test_modules_log.c
 * @brief   Unit test for logging subsystem
 *
 * @addtogroup tests
 * @{
 */
#include <string.h>

#include <ch.h>

#include <main.h>
#include "modules/log.h"

/* *****************************************************************************
 * Mocks
***************************************************************************** */
static log_msg_t msg1, msg2;
static bool called1 = false, called2 = false;
static log_msg_t msg_send;
static bool msg_called = false;

static msg_t chMBFetchTimeout(void *mbox, msg_t *msg, uint32_t timeout)
{
    (void) mbox;
    (void) msg;
    (void) timeout;
    return (msg_t) 0;
}

static void *chPoolAlloc(char *mp)
{
    static log_msg_t msg;
    (void) mp;
    return (void *)&msg;
}

static msg_t chMBPostTimeout(void *mbp, msg_t msg, uint32_t timeout)
{
    (void) mbp;
    (void) timeout;

    memcpy (&msg_send, (uint8_t *)msg, sizeof(msg_send));
    msg_called = true;
    return (msg_t) 0;
}

static void chPoolFree(char *mp, void *objp)
{
    (void) mp;
    (void) objp;
}

static void subscr1(const log_msg_t *msg)
{
    memcpy(&msg1, msg, sizeof(msg1));
    called1 = true;
}

static void subscr2(const log_msg_t *msg)
{
    memcpy(&msg2, msg, sizeof(msg2));
    called2 = true;
}

/* *****************************************************************************
 * Tests
***************************************************************************** */
#include "modules/log.c"

TEST_GROUP(Log);

TEST_SETUP(Log)
{
    Log_Init();
    /* Clear subscribtions */
    memset(logi_subscriptions, 0x00,
            sizeof(logi_subscriptions[0])*LOG_CALLBACKS_NUM);
    called1 = false;
    called2 = false;
    msg_called = false;
}

TEST_TEAR_DOWN(Log)
{

}

TEST(Log, Subscriptions)
{
    log_severity_t severities[LOG_SOURCE_COUNT];

    assert_should_fail = true;
    Log_Subscribe(NULL, NULL, false);
    assert_should_fail = false;

    TEST_ASSERT_EQUAL(Logi_GetHighestSeverity(), LOG_SEVERITY_NONE);

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severities[i] = LOG_SEVERITY_NONE;
    }

    severities[LOG_SOURCE_COUNT-2] = LOG_SEVERITY_WARNING;
    TEST_ASSERT_TRUE(Log_Subscribe(subscr1, severities, false));
    TEST_ASSERT_EQUAL(LOG_SEVERITY_WARNING, Logi_GetHighestSeverity());

    severities[LOG_SOURCE_COUNT-2] = LOG_SEVERITY_ERROR;
    TEST_ASSERT_TRUE(Log_Subscribe(subscr2, severities, false));
    TEST_ASSERT_EQUAL(LOG_SEVERITY_WARNING, Logi_GetHighestSeverity() );

    severities[LOG_SOURCE_COUNT-2] = LOG_SEVERITY_DEBUG;
    TEST_ASSERT_TRUE(Log_UpdateSubscription(subscr1, severities));
    TEST_ASSERT_EQUAL(LOG_SEVERITY_DEBUG, Logi_GetHighestSeverity());

    TEST_ASSERT_TRUE(Log_Unsubscribe(subscr1));
    TEST_ASSERT_EQUAL(LOG_SEVERITY_ERROR, Logi_GetHighestSeverity());

    TEST_ASSERT_FALSE(Log_Unsubscribe(subscr1));
}

TEST(Log, SendLog)
{
    log_msg_t msg;
    log_severity_t severities[LOG_SOURCE_COUNT];

    assert_should_fail = true;
    Logi_SendLog(NULL);
    assert_should_fail = false;

    msg.time = 123;
    msg.severity = LOG_SEVERITY_WARNING;
    msg.src = LOG_SOURCE_SYSTEM;
    msg.module = LOG_MODULE_MYSELF;
    sprintf(msg.msg, "Hello World!");

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severities[i] = LOG_SEVERITY_NONE;
    }

    Logi_SendLog(&msg);
    TEST_ASSERT_FALSE(called1);
    TEST_ASSERT_FALSE(called2);

    severities[LOG_SOURCE_SYSTEM] = LOG_SEVERITY_ERROR;
    TEST_ASSERT_TRUE(Log_Subscribe(subscr1, severities, false));

    severities[LOG_SOURCE_SYSTEM] = LOG_SEVERITY_INFO;
    severities[LOG_SOURCE_MODULE] = LOG_SEVERITY_WARNING;
    TEST_ASSERT_TRUE(Log_Subscribe(subscr2, severities, false));

    Logi_SendLog(&msg);
    TEST_ASSERT_FALSE(called1);
    TEST_ASSERT_TRUE(called2);
    TEST_ASSERT_EQUAL(LOG_SEVERITY_WARNING, msg2.severity);
    TEST_ASSERT_EQUAL(LOG_SOURCE_SYSTEM, msg2.src);
    TEST_ASSERT_EQUAL(123, msg2.time);
    TEST_ASSERT_EQUAL_STRING("Hello World!", msg2.msg);

    called1 = false;
    called2 = false;
    msg.severity = LOG_SEVERITY_ERROR;
    Logi_SendLog(&msg);
    TEST_ASSERT_TRUE(called1);
    TEST_ASSERT_TRUE(called2);
    TEST_ASSERT_EQUAL(LOG_SEVERITY_ERROR, msg1.severity);
    TEST_ASSERT_EQUAL(LOG_SOURCE_SYSTEM, msg1.src);
    TEST_ASSERT_EQUAL(LOG_SEVERITY_ERROR, msg2.severity);
    TEST_ASSERT_EQUAL(LOG_SOURCE_SYSTEM, msg2.src);

    called1 = false;
    called2 = false;
    msg.severity = LOG_SEVERITY_ERROR;
    msg.src = LOG_SOURCE_MODULE;
    Logi_SendLog(&msg);
    TEST_ASSERT_FALSE(called1);
    TEST_ASSERT_TRUE(called2);
    TEST_ASSERT_EQUAL(LOG_SEVERITY_ERROR, msg2.severity);
    TEST_ASSERT_EQUAL(LOG_SOURCE_MODULE, msg2.src);
}

TEST(Log, SendLogExternal)
{
    log_msg_t msg;
    log_severity_t severities[LOG_SOURCE_COUNT];

    msg.time = 123;
    msg.severity = LOG_SEVERITY_WARNING;
    msg.src = LOG_SOURCE_SYSTEM;
    msg.module = LOG_MODULE_PSU;
    sprintf(msg.msg, "Hello World!");

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severities[i] = LOG_SEVERITY_DEBUG;
    }

    TEST_ASSERT_TRUE(Log_Subscribe(subscr1, severities, false));
    TEST_ASSERT_TRUE(Log_Subscribe(subscr2, severities, true));

    Logi_SendLog(&msg);
    TEST_ASSERT_FALSE(called1);
    TEST_ASSERT_TRUE(called2);
}

TEST(Log, AddEntry)
{
    log_severity_t severities[LOG_SOURCE_COUNT];

    assert_should_fail = true;
    Log_Debug(LOG_SOURCE_SYSTEM, NULL);
    Log_Warn(LOG_SOURCE_SYSTEM, NULL);
    Log_Info(LOG_SOURCE_SYSTEM, NULL);
    Log_Error(LOG_SOURCE_SYSTEM, NULL);
    assert_should_fail = false;

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severities[i] = LOG_SEVERITY_NONE;
    }

    severities[LOG_SOURCE_SYSTEM] = LOG_SEVERITY_WARNING;
    TEST_ASSERT_TRUE(Log_Subscribe(subscr1, severities, false));

    Log_Debug(LOG_SOURCE_SYSTEM, "Hello World %d", 123);
    TEST_ASSERT_FALSE(msg_called);

    Log_Error(LOG_SOURCE_SYSTEM, "Hello World %d", 123);
    TEST_ASSERT_TRUE(msg_called);

    TEST_ASSERT_EQUAL(LOG_SEVERITY_ERROR, msg_send.severity);
    TEST_ASSERT_EQUAL(LOG_SOURCE_SYSTEM, msg_send.src);
    TEST_ASSERT_GREATER_THAN(0, msg_send.time);
    TEST_ASSERT_EQUAL_STRING("Hello World 123", msg_send.msg);

}

TEST(Log, GetSubscription)
{
    log_severity_t severities[LOG_SOURCE_COUNT];
    log_severity_t results[LOG_SOURCE_COUNT];

    assert_should_fail = true;
    Log_GetSubscription(NULL, NULL);
    assert_should_fail = false;

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severities[i] = i % LOG_SEVERITY_COUNT;
    }

    TEST_ASSERT_TRUE(Log_Subscribe(subscr1, severities, false));
    memset(results, 0xff, LOG_SEVERITY_COUNT);
    TEST_ASSERT_TRUE(Log_GetSubscription(subscr1, results));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(severities, results, LOG_SOURCE_COUNT);

    TEST_ASSERT_FALSE(Log_GetSubscription(subscr2, results));
}

TEST_GROUP_RUNNER(Log)
{
    RUN_TEST_CASE(Log, Subscriptions);
    RUN_TEST_CASE(Log, SendLog);
    RUN_TEST_CASE(Log, SendLogExternal);
    RUN_TEST_CASE(Log, AddEntry);
    RUN_TEST_CASE(Log, GetSubscription);
}

void Log_RunTests(void)
{
    RUN_TEST_GROUP(Log);
}

/** @} */
