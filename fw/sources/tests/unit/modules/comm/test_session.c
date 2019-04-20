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
 * @file    test_session.c
 * @brief   Unit test for communication module, session link layer
 *
 * @addtogroup tests
 * @{
 */

#include <main.h>
#include <string.h>

#include "modules/comm/session.c"

/* *****************************************************************************
 * Mocks
***************************************************************************** */
static void Log_Debug(log_src_t src, const char *format, ...)
{
    (void) src;
    (void) format;
}

static void Log_Info(log_src_t src, const char *format, ...)
{
    (void) src;
    (void) format;
}

static void Log_Warn(log_src_t src, const char *format, ...)
{
    (void) src;
    (void) format;
}

static void Log_Error(log_src_t src, const char *format, ...)
{
    (void) src;
    (void) format;
}

static bool Comm_HandlePacket(comm_node_t dest, const comm_packet_t *packet,
        comm_send_cb_t send_iface)
{
    (void) dest;
    (void) packet;
    (void) send_iface;
    return true;
}


/* *****************************************************************************
 * Helpers
***************************************************************************** */
static bool sendfunc(comm_node_t dest, comm_priority_t priority,
        const uint8_t *payload, uint8_t len)
{

    (void) dest;
    (void) priority;
    (void) payload;
    (void) len;

    return true;
}

/* *****************************************************************************
 * Tests
***************************************************************************** */
TEST_GROUP(Comm_Session);

TEST_SETUP(Comm_Session)
{
}

TEST_TEAR_DOWN(Comm_Session)
{
}

TEST(Comm_Session, HandlePayload)
{
    uint8_t buf[3] = {0, 11, 12};
    comm_packet_t rsp;
    comm_cmd_t cmd;

    commi_session.response = &rsp;

    assert_should_fail = true;
    Comm_HandlePayload(0, 0, NULL, 1, sendfunc);
    Comm_HandlePayload(0, 0, buf, 1, NULL);
    assert_should_fail = false;

    TEST_ASSERT_FALSE(Comm_HandlePayload(0, 0, buf, 0, sendfunc));

    /* not part of previous communication */
    cmd.id = 12;
    cmd.response = 0;
    cmd.error = 0;
    buf[0] = cmd.intval;
    TEST_ASSERT_TRUE(Comm_HandlePayload(0, 0, buf, 3, sendfunc));

    /* Response to previous communication */
    cmd.response = 1;
    buf[0] = cmd.intval;
    commi_session.dest = 4;
    commi_session.cmd.id = 14;
    TEST_ASSERT_FALSE(Comm_HandlePayload(4, 0, buf, 3, sendfunc));

    commi_session.cmd.id = 12;
    TEST_ASSERT_FALSE(Comm_HandlePayload(3, 0, buf, 3, sendfunc));

    TEST_ASSERT_TRUE(Comm_HandlePayload(4, 2, buf, 3, sendfunc));
    TEST_ASSERT_EQUAL(cmd.intval, commi_session.response->cmd.intval);
    TEST_ASSERT_EQUAL(4, commi_session.response->node);
    TEST_ASSERT_EQUAL(2, commi_session.response->len);
    TEST_ASSERT_EQUAL(11, commi_session.response->payload[0]);
    TEST_ASSERT_EQUAL(12, commi_session.response->payload[1]);
}

TEST(Comm_Session, SendPayload)
{
    comm_packet_t packet;

    assert_should_fail = true;
    Comm_SendPayload(NULL, NULL, sendfunc);
    Comm_SendPayload(&packet, NULL, NULL);
    assert_should_fail = false;

    TEST_ASSERT_EQUAL(COMM_OK, Comm_SendPayload(&packet, NULL, sendfunc));


}

TEST_GROUP_RUNNER(Comm_Session)
{
    RUN_TEST_CASE(Comm_Session, HandlePayload);
    RUN_TEST_CASE(Comm_Session, SendPayload);
}

void Comm_Session_RunTests(void)
{
    RUN_TEST_GROUP(Comm_Session);
}

/** @} */
