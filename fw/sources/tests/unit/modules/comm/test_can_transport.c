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
 * @file    test_can_link.c
 * @brief   Unit test for communication module, CAN link layer
 *
 * @addtogroup tests
 * @{
 */

#include <string.h>

#include <main.h>
#include "modules/comm/can_transport.c"

static uint8_t recbuf[256];
static uint8_t bytes_received = 0;
static bool payload_handled = false;
static comm_node_t node_src, node_dest;

static uint8_t txbuf[256];
static uint16_t send_bytes;
static uint16_t send_calls;

static CANRxFrame can_frame;

/* *****************************************************************************
 * Mocks
***************************************************************************** */
static bool Comm_HandlePayload(comm_node_t src, comm_node_t dest,
        const uint8_t *payload, uint8_t len, comm_send_cb_t send_frame)
{
    TEST_ASSERT_NOT_NULL(payload);

    memcpy(recbuf, payload, len);
    bytes_received = len;
    payload_handled = true;
    node_src = src;
    node_dest = dest;

    TEST_ASSERT_EQUAL_PTR(send_frame, Comm_CanSend);
    return true;
}

static bool Cand_SendFrame(const CANTxFrame *frame)
{
    comm_can_id_t id;

    TEST_ASSERT_NOT_NULL(frame);

    memcpy(&txbuf[send_bytes], frame->data8, frame->DLC);
    send_bytes += frame->DLC;
    id = Commi_SID2Struct(frame->SID);
    node_src = id.src;
    node_dest = id.dest;
    send_calls++;
    return true;
}

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

static uint8_t CRC8(const uint8_t *buf, uint32_t len)
{
    (void) buf;
    (void) len;

    return 0xab;
}

static void Cand_RegisterRxCb(cand_rx_cb_t cb)
{
    (void) cb;
}

static void Cand_Init(uint32_t baudrate, const cand_filter_t *filters,
        uint8_t count)
{
    (void) baudrate;
    (void) filters;
    (void) count;
}
/* *****************************************************************************
 * Helpers
***************************************************************************** */
static void setMultipart(void)
{
    comm_can_id_t id;
    id = Commi_SID2Struct(can_frame.SID);
    id.multipart = 1;
    can_frame.SID = Commi_Struct2SID(id);
}

static void setSrc(uint8_t src)
{
    comm_can_id_t id;
    id = Commi_SID2Struct(can_frame.SID);
    id.src = src;
    can_frame.SID = Commi_Struct2SID(id);
}

/* *****************************************************************************
 * Tests
***************************************************************************** */
TEST_GROUP(Comm_CANTransport);

TEST_SETUP(Comm_CANTransport)
{
    comm_can_id_t id;

    bytes_received = 0;
    payload_handled = false;
    memset(recbuf, 0x00, sizeof(txbuf));
    node_src = node_dest = COMM_NODE_BROADCAST;

    memset(txbuf, 0x00, sizeof(txbuf));
    send_bytes = 0;
    send_calls = 0;

    can_frame.DLC = 0;      /* length */
    can_frame.RTR = 0;      /* remote frame */
    can_frame.IDE = 0;      /* extended identifier */
    id.src = COMM_NODE_UNKNOWN;
    id.dest = COMM_MY_ID;
    id.priority = 0;
    id.multipart = 0;
    can_frame.SID = Commi_Struct2SID(id);
    memset(can_frame.data8, 0x00, 8);

    /* Clear mailboxes - defined in can_link.c */
    memset(cani_rx_mbox, 0x00, sizeof(cani_rx_mbox));
}

TEST_TEAR_DOWN(Comm_CANTransport)
{
}

TEST(Comm_CANTransport, SID2Struct)
{
    comm_can_id_t id = Commi_SID2Struct(0x6b5);

    TEST_ASSERT_EQUAL(1, id.multipart);
    TEST_ASSERT_EQUAL(0x0a, id.dest);
    TEST_ASSERT_EQUAL(0x05, id.src);
    TEST_ASSERT_EQUAL(0x03, id.priority);
}

TEST(Comm_CANTransport, Struct2SID)
{
    comm_can_id_t id;

    id.multipart = 1;
    id.dest = 0x0A;
    id.src = 0x05;
    id.priority = 0x03;

    TEST_ASSERT_EQUAL(0x6b5, Commi_Struct2SID(id));
}

TEST(Comm_CANTransport, ProcessFrameInvalid)
{
    assert_should_fail = true;
    Commi_CanProcessFrame(NULL);
    assert_should_fail = false;

    /* Remote requests not supported */
    can_frame.RTR = 1;
    TEST_ASSERT_FALSE(Commi_CanProcessFrame(&can_frame));

    /* Extended identifier not supported */
    can_frame.RTR = 0;
    can_frame.IDE = 1;
    TEST_ASSERT_FALSE(Commi_CanProcessFrame(&can_frame));
}

TEST(Comm_CANTransport, ProcessFrameSingleShort)
{
    can_frame.DLC = 1;
    can_frame.data8[0] = 0xcd;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));

    TEST_ASSERT_TRUE(payload_handled);
    TEST_ASSERT_EQUAL(1, bytes_received);
    TEST_ASSERT_EQUAL(0xcd, recbuf[0]);
    TEST_ASSERT_EQUAL_HEX8(COMM_NODE_UNKNOWN, node_src);
    TEST_ASSERT_EQUAL_HEX8(COMM_MY_ID, node_dest);
}

TEST(Comm_CANTransport, ProcessFrameSingleLong)
{
    uint8_t data[] = {0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x89, 0xff};

    can_frame.DLC = 8;
    memcpy(can_frame.data8, data, 8);

    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));

    TEST_ASSERT_TRUE(payload_handled);
    TEST_ASSERT_EQUAL(8, bytes_received);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, recbuf, 8);
}

TEST(Comm_CANTransport, ProcessFrameMultipartCRCValid)
{
    uint8_t data[] = {0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a,
    0xaa, 0xbb, 0xcc, 0xee, 0xff, 0x1a, 0x2b};
    setMultipart();

    can_frame.DLC = 8;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 15;
    memcpy(&can_frame.data8[2], data, 6);
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);

    can_frame.data8[0] = 0x01;
    memcpy(&can_frame.data8[1], data+6, 7);
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);

    can_frame.DLC = 4;
    can_frame.data8[0] = 0x02;
    memcpy(&can_frame.data8[1], data+13, 2);
    can_frame.data8[3] = 0xab;  /* crc */
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));

    TEST_ASSERT_TRUE(payload_handled);
    TEST_ASSERT_EQUAL(15, bytes_received);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, recbuf, 15);
    TEST_ASSERT_EQUAL_HEX8(COMM_NODE_UNKNOWN, node_src);
    TEST_ASSERT_EQUAL_HEX8(COMM_MY_ID, node_dest);
}

TEST(Comm_CANTransport, ProcessFrameMultipartCRCSeparated)
{
    uint8_t data[] = {0xab, 0xcd, 0xef, 0x12};
    setMultipart();

    can_frame.DLC = 2;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 0x04;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);

    can_frame.DLC = 5;
    can_frame.data8[0] = 0x01;  /* sequence number */
    can_frame.data8[1] = 0xab;
    can_frame.data8[2] = 0xcd;
    can_frame.data8[3] = 0xef;
    can_frame.data8[4] = 0x12;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);

    can_frame.DLC = 2;
    can_frame.data8[0] = 0x02;
    can_frame.data8[1] = 0xab;  /* crc */
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));

    TEST_ASSERT_TRUE(payload_handled);
    TEST_ASSERT_EQUAL(4, bytes_received);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, recbuf, 4);
}

TEST(Comm_CANTransport, ProcessFrameMultipartCRCInvalid)
{
    setMultipart();

    can_frame.DLC = 3;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 0x04;
    can_frame.data8[2] = 0xab;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);

    can_frame.DLC = 5;
    can_frame.data8[0] = 0x01;
    can_frame.data8[1] = 0xcd;
    can_frame.data8[2] = 0xef;
    can_frame.data8[3] = 0x12;
    can_frame.data8[4] = 0xff;  /* invalid crc */
    TEST_ASSERT_FALSE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);
}

TEST(Comm_CANTransport, ProcessFrameMultipartZeroLength)
{
    setMultipart();

    can_frame.DLC = 3;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 0x00;
    can_frame.data8[2] = 0xab;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_TRUE(payload_handled);
    TEST_ASSERT_EQUAL(0, bytes_received);
}

/** Replace previous transmission */
TEST(Comm_CANTransport, ProcessFrameMultipartRestart)
{
    setMultipart();

    can_frame.DLC = 3;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 0x04;
    can_frame.data8[2] = 0xab;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);

    can_frame.DLC = 5;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 0x02;
    can_frame.data8[2] = 0x12;
    can_frame.data8[3] = 0x34;
    can_frame.data8[4] = 0xab;  /* valid crc */
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_TRUE(payload_handled);
}

TEST(Comm_CANTransport, ProcessFrameMultipartWrongOrder)
{
    setMultipart();

    can_frame.DLC = 3;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 0x04;
    can_frame.data8[2] = 0xab;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);

    can_frame.DLC = 5;
    can_frame.data8[0] = 0x02;
    can_frame.data8[1] = 0x02;
    can_frame.data8[2] = 0x12;
    can_frame.data8[3] = 0x34;
    can_frame.data8[4] = 0xab;  /* valid crc */
    TEST_ASSERT_FALSE(Commi_CanProcessFrame(&can_frame));

    /* mailbox should be scraped after unsuccessfull data */
    can_frame.DLC = 2;
    can_frame.data8[0] = 0x01;
    can_frame.data8[1] = 0x12;
    TEST_ASSERT_FALSE(Commi_CanProcessFrame(&can_frame));
}

TEST(Comm_CANTransport, ProcessFrameMultipartFrameTooLong)
{
    setMultipart();

    can_frame.DLC = 3;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 0x04;
    can_frame.data8[2] = 0xab;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);

    can_frame.DLC = 6;
    can_frame.data8[0] = 0x01;
    can_frame.data8[1] = 0xcd;
    can_frame.data8[2] = 0x12;
    can_frame.data8[3] = 0x34;
    can_frame.data8[4] = 0xab;  /* valid crc */
    can_frame.data8[5] = 0xff;  /* bullshit over length */
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_TRUE(payload_handled);
}

/** Replace previous transmission */
TEST(Comm_CANTransport, ProcessFrameMultipartFull)
{
    setMultipart();

    /* fill the slots */
    for (int i = 0; i < COMM_CAN_MBOX_SLOTS; i++) {
        setSrc(i);
        can_frame.DLC = 3;
        can_frame.data8[0] = 0x00;
        can_frame.data8[1] = 0x02;
        can_frame.data8[2] = 0x12;
        TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
        TEST_ASSERT_FALSE(payload_handled);
    }

    setSrc(COMM_CAN_MBOX_SLOTS);
    can_frame.DLC = 3;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 0x02;
    can_frame.data8[2] = 0x12;
    TEST_ASSERT_FALSE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);

    /* finish one of the transmissions */
    setSrc(0x01);
    can_frame.DLC = 3;
    can_frame.data8[0] = 0x01;
    can_frame.data8[1] = 0x34;
    can_frame.data8[2] = 0xab;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_TRUE(payload_handled);

    /* and try to send data again */
    payload_handled = false;
    setSrc(COMM_CAN_MBOX_SLOTS);
    can_frame.DLC = 3;
    can_frame.data8[0] = 0x00;
    can_frame.data8[1] = 0x02;
    can_frame.data8[2] = 0x12;
    TEST_ASSERT_TRUE(Commi_CanProcessFrame(&can_frame));
    TEST_ASSERT_FALSE(payload_handled);
}


TEST(Comm_CANTransport, SendInvalid)
{
    uint8_t buf[2];

    assert_should_fail = true;
    Comm_CanSend(0, 0, NULL, 10);
    assert_should_fail = false;

    TEST_ASSERT_FALSE(Comm_CanSend(0, 0, buf, 0));
}

TEST(Comm_CANTransport, SendShort)
{
    uint8_t data[] = {0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a};
    TEST_ASSERT_TRUE(Comm_CanSend(COMM_MY_ID, COMM_NODE_BROADCAST, data, 8));
    TEST_ASSERT_EQUAL(1, send_calls);
    TEST_ASSERT_EQUAL(8, send_bytes);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, txbuf, 8);
}

TEST(Comm_CANTransport, SendLong)
{
    uint8_t data[] = {0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a,
    0xaa, 0xbb, 0xcc, 0xee, 0xff, 0x1a, 0x1b};
    TEST_ASSERT_TRUE(Comm_CanSend(COMM_MY_ID, COMM_NODE_BROADCAST, data, 15));
    TEST_ASSERT_EQUAL(3, send_calls);
    TEST_ASSERT_EQUAL(15 + 2 + 1 + 2, send_bytes);
    TEST_ASSERT_EQUAL_HEX8(0x00, txbuf[0]);
    TEST_ASSERT_EQUAL(15, txbuf[1]);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, &txbuf[2], 6);
    TEST_ASSERT_EQUAL_HEX8(0x01, txbuf[8]);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&data[6], &txbuf[9], 7);
    TEST_ASSERT_EQUAL_HEX8(0x02, txbuf[16]);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&data[13], &txbuf[17], 2);
    TEST_ASSERT_EQUAL_HEX8(0xab, txbuf[19]);
}

TEST(Comm_CANTransport, SendLongCrcSeparated)
{
    uint8_t data[] = {0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a,
    0xaa, 0xbb, 0xcc, 0xee, 0xff};
    TEST_ASSERT_TRUE(Comm_CanSend(COMM_MY_ID, COMM_NODE_BROADCAST, data, 13));
    TEST_ASSERT_EQUAL(3, send_calls);
    TEST_ASSERT_EQUAL(13 + 2 + 1 + 2, send_bytes);
    TEST_ASSERT_EQUAL_HEX8(0x00, txbuf[0]);
    TEST_ASSERT_EQUAL(13, txbuf[1]);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, &txbuf[2], 6);
    TEST_ASSERT_EQUAL_HEX8(0x01, txbuf[8]);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(&data[6], &txbuf[9], 7);
    TEST_ASSERT_EQUAL_HEX8(0x02, txbuf[16]);
    TEST_ASSERT_EQUAL_HEX8(0xab, txbuf[17]);
}

TEST_GROUP_RUNNER(Comm_CANTransport)
{
    RUN_TEST_CASE(Comm_CANTransport, SID2Struct);
    RUN_TEST_CASE(Comm_CANTransport, Struct2SID);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameInvalid);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameSingleShort);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameSingleLong);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameMultipartCRCValid);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameMultipartCRCSeparated);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameMultipartCRCInvalid);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameMultipartZeroLength);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameMultipartRestart);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameMultipartWrongOrder);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameMultipartFrameTooLong);
    RUN_TEST_CASE(Comm_CANTransport, ProcessFrameMultipartFull);

    RUN_TEST_CASE(Comm_CANTransport, SendInvalid);
    RUN_TEST_CASE(Comm_CANTransport, SendShort);
    RUN_TEST_CASE(Comm_CANTransport, SendLong);
    RUN_TEST_CASE(Comm_CANTransport, SendLongCrcSeparated);
}

void Comm_CanTransport_RunTests(void)
{
    RUN_TEST_GROUP(Comm_CANTransport);
}

/** @} */
