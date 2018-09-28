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
 * @file    test_uart_link.c
 * @brief   Unit test for communication module, uart link layer
 *
 * @addtogroup tests
 * @{
 */

#include <string.h>

#include <main.h>
#include "modules/comm/uart_link.c"

static uint8_t recbuf[30];
static uint8_t received = 0;
static bool payload_handled = false;

/* *****************************************************************************
 * Mocks
***************************************************************************** */
static void Comm_HandlePayload(const uint8_t *payload, uint8_t len,
        comm_send_cb_t send_frame)
{
    memcpy(recbuf, payload, len);
    received = len;
    payload_handled = true;

    TEST_ASSERT_EQUAL_PTR(send_frame, Comm_Uart_Send);
}

static void Uartd_RegisterRxCb(uartd_rx_cb_t cb)
{
    (void) cb;
}

static void Uartd_SendByte(uint8_t byte)
{
    recbuf[received++] = byte;
}

static void Uartd_Init(uint32_t baudrate)
{
    (void) baudrate;
}

static void Log_Debug(log_src_t src, const char *format, ...)
{
    (void) src;
    (void) format;
}

static void Log_Warn(log_src_t src, const char *format, ...)
{
    (void) src;
    (void) format;
}

static uint8_t CRC8_Add(uint8_t buf, uint8_t crc)
{
    (void) buf;
    (void) crc;

    return 0xab;
}

/* *****************************************************************************
 * Tests
***************************************************************************** */
TEST_GROUP(Comm_UartLink);

TEST_SETUP(Comm_UartLink)
{
    received = 0;
    payload_handled = false;
    memset(recbuf, 0x00, 1024);
}

TEST_TEAR_DOWN(Comm_UartLink)
{

}

TEST(Comm_UartLink, ProcessByteValid)
{
    /* Incorrect bytes */
    Comm_Uart_ProcessByte(0x12);
    TEST_ASSERT_FALSE(payload_handled);
    Comm_Uart_ProcessByte(0x19);
    TEST_ASSERT_FALSE(payload_handled);

    /* Correct payload */
    Comm_Uart_ProcessByte(0xff);
    TEST_ASSERT_FALSE(payload_handled);
    Comm_Uart_ProcessByte(0x12);
    TEST_ASSERT_FALSE(payload_handled);

    for (int i = 0; i < 18; i++) {
        Comm_Uart_ProcessByte(i);
        TEST_ASSERT_FALSE(payload_handled);
    }

    /* crc */
    Comm_Uart_ProcessByte(0xab);
    TEST_ASSERT_TRUE(payload_handled);
    TEST_ASSERT_EQUAL(18, received);

    for (int i = 0; i < 18; i++) {
        TEST_ASSERT_EQUAL_HEX8(i, recbuf[i]);
    }
}

TEST(Comm_UartLink, ProcessByteInvalid)
{
    Comm_Uart_ProcessByte(0xff);
    TEST_ASSERT_FALSE(payload_handled);
    Comm_Uart_ProcessByte(0x02);
    TEST_ASSERT_FALSE(payload_handled);

    Comm_Uart_ProcessByte(0xab);
    TEST_ASSERT_FALSE(payload_handled);
    Comm_Uart_ProcessByte(0xcd);
    TEST_ASSERT_FALSE(payload_handled);

    /* invalid crc */
    Comm_Uart_ProcessByte(0xcd);
    TEST_ASSERT_FALSE(payload_handled);
}

TEST(Comm_UartLink, SendPayload)
{
    uint8_t payload[18];
    for (int i = 0; i < 18; i++) {
        payload[i] = i;
    }

    TEST_ASSERT_TRUE(Comm_Uart_Send(payload, 18));
    TEST_ASSERT_EQUAL(21, received);
    TEST_ASSERT_EQUAL_HEX8(0xff, recbuf[0]);
    TEST_ASSERT_EQUAL(18, recbuf[1]);
    for (int i = 0; i < 18; i++) {
        TEST_ASSERT_EQUAL_HEX8(i, recbuf[i+2]);
    }
    TEST_ASSERT_EQUAL(0xab, recbuf[20]);
}

TEST_GROUP_RUNNER(Comm_UartLink)
{
    RUN_TEST_CASE(Comm_UartLink, ProcessByteValid);
    RUN_TEST_CASE(Comm_UartLink, ProcessByteInvalid);
    RUN_TEST_CASE(Comm_UartLink, SendPayload);
}

void Comm_UartLink_RunTests(void)
{
    RUN_TEST_GROUP(Comm_UartLink);
}
