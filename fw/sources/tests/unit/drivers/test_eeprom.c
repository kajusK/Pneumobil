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
 * @file    test_eeprom.c
 * @brief   Unit test for eeprom driver
 *
 * @addtogroup tests
 * @{
 */

#include <string.h>

#include <main.h>
#include "drivers/eeprom.c"

static uint8_t buf[256];
static uint8_t bytes_received[10];
static uint16_t address_written[10];
static int call_count = 0;
static int buf_pos = 0;

/* *****************************************************************************
 * Mocks
***************************************************************************** */
static bool I2Cd_Transceive(uint8_t address, const uint8_t *txbuf,
        size_t txbytes, uint8_t *rxbuf, size_t rxbytes)
{
    (void) address;
    (void) rxbuf;
    TEST_ASSERT_EQUAL(0, rxbytes);

    /* without address bytes */
    bytes_received[call_count] = txbytes - 2;
    address_written[call_count] = (txbuf[0] << 8) | txbuf[1];
    memcpy(&buf[buf_pos], &txbuf[2], txbytes - 2);
    call_count++;
    return true;
}

/* *****************************************************************************
 * Tests
***************************************************************************** */
TEST_GROUP(Eeprom);

TEST_SETUP(Eeprom)
{
    memset(buf, 0x00, 256);
    memset(bytes_received, 0x00, 10);
    memset(address_written, 0x00, sizeof(address_written));
    call_count = 0;
    buf_pos = 0;
}

TEST_TEAR_DOWN(Eeprom)
{

}

/*
 * Writing single byte
 */
TEST(Eeprom, SendSingle)
{
    uint8_t data = 0xde;
    TEST_ASSERT_TRUE(Eepromd_Write(0x0123, &data, 1));

    TEST_ASSERT_EQUAL(1, call_count);
    TEST_ASSERT_EQUAL(1, bytes_received[0]);
    TEST_ASSERT_EQUAL_HEX16(0x123, address_written[0]);
}

/*
 * Writing few bytes not crossing page boundary
 */
TEST(Eeprom, SendSmallNotCrossing)
{
    uint8_t data[EEPROM_PAGE_SIZE/2];

    for (int i = 0; i < EEPROM_PAGE_SIZE/2; i++) {
        data[i] = i;
    }

    TEST_ASSERT_TRUE(Eepromd_Write(EEPROM_PAGE_SIZE*2+4, data, EEPROM_PAGE_SIZE/2));

    TEST_ASSERT_EQUAL(1, call_count);
    TEST_ASSERT_EQUAL(EEPROM_PAGE_SIZE/2, bytes_received[0]);
    TEST_ASSERT_EQUAL_HEX16(EEPROM_PAGE_SIZE*2+4, address_written[0]);

    for (int i = 0; i < EEPROM_PAGE_SIZE/2; i++) {
        TEST_ASSERT_EQUAL_HEX8(i, data[i]);
    }
}

/*
 * Writing few bytes crossing page boundary
 */
TEST(Eeprom, SendSmallCrossing)
{
    uint8_t data[EEPROM_PAGE_SIZE/2];

    for (int i = 0; i < EEPROM_PAGE_SIZE/2; i++) {
        data[i] = i;
    }

    TEST_ASSERT_TRUE(Eepromd_Write(EEPROM_PAGE_SIZE*2-5, data, EEPROM_PAGE_SIZE/2));

    TEST_ASSERT_EQUAL(2, call_count);
    TEST_ASSERT_EQUAL(5, bytes_received[0]);
    TEST_ASSERT_EQUAL(EEPROM_PAGE_SIZE/2-5, bytes_received[1]);
    TEST_ASSERT_EQUAL_HEX16(EEPROM_PAGE_SIZE*2-5, address_written[0]);
    TEST_ASSERT_EQUAL_HEX16(EEPROM_PAGE_SIZE*2, address_written[1]);

    for (int i = 0; i < EEPROM_PAGE_SIZE/2; i++) {
        TEST_ASSERT_EQUAL_HEX8(i, data[i]);
    }
}

/*
 * Writing many bytes acrossing several pages
 */
TEST(Eeprom, SendFewPages)
{
    uint8_t data[EEPROM_PAGE_SIZE*3];

    for (int i = 0; i < EEPROM_PAGE_SIZE*3; i++) {
        data[i] = i;
    }

    TEST_ASSERT_TRUE(Eepromd_Write(EEPROM_PAGE_SIZE*2-5, data, EEPROM_PAGE_SIZE*3));

    TEST_ASSERT_EQUAL(4, call_count);
    TEST_ASSERT_EQUAL(5, bytes_received[0]);
    TEST_ASSERT_EQUAL(EEPROM_PAGE_SIZE, bytes_received[1]);
    TEST_ASSERT_EQUAL(EEPROM_PAGE_SIZE, bytes_received[2]);
    TEST_ASSERT_EQUAL(EEPROM_PAGE_SIZE-5, bytes_received[3]);

    TEST_ASSERT_EQUAL_HEX16(EEPROM_PAGE_SIZE*2-5, address_written[0]);
    TEST_ASSERT_EQUAL_HEX16(EEPROM_PAGE_SIZE*2, address_written[1]);
    TEST_ASSERT_EQUAL_HEX16(EEPROM_PAGE_SIZE*3, address_written[2]);
    TEST_ASSERT_EQUAL_HEX16(EEPROM_PAGE_SIZE*4, address_written[3]);

    for (int i = 0; i < EEPROM_PAGE_SIZE*3; i++) {
        TEST_ASSERT_EQUAL_HEX8(i, data[i]);
    }
}


TEST_GROUP_RUNNER(Eeprom)
{
    RUN_TEST_CASE(Eeprom, SendSingle);
    RUN_TEST_CASE(Eeprom, SendSmallNotCrossing);
    RUN_TEST_CASE(Eeprom, SendSmallCrossing);
    RUN_TEST_CASE(Eeprom, SendFewPages);
}

void Eeprom_RunTests(void)
{
    RUN_TEST_GROUP(Eeprom);
}
