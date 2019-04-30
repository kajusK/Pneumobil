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
 * @brief   System test for eeprom driver
 *
 * @addtogroup tests-system
 * @{
 */

#include "drivers/eeprom.h"

/* *****************************************************************************
 * Tests
***************************************************************************** */
TEST_GROUP(Eeprom);

TEST_SETUP(Eeprom)
{
}

TEST_TEAR_DOWN(Eeprom)
{

}

TEST(Eeprom, SendSingle)
{
    uint8_t val;
    uint8_t buf;

    val = 0xab;
    TEST_ASSERT_TRUE(Eepromd_Write(0x0123, &val, 1);
    TEST_ASSERT_TRUE(Eepromd_Read(0x0123, &buf, 1);
    TEST_ASSERT_EQUAL(0xab, buf);

    val = 0xcd;
    TEST_ASSERT_TRUE(Eepromd_Write(0x0123, &val, 1);
    TEST_ASSERT_TRUE(Eepromd_Read(0x0123, &buf, 1);
    TEST_ASSERT_EQUAL(0xcd, buf);
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
    memset(data, 0x00, EEPROM_PAGE_SIZE/2);
    TEST_ASSERT_TRUE(Eepromd_Read(EEPROM_PAGE_SIZE*2+4, data, EEPROM_PAGE_SIZE/2));

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
    memset(data, 0x00, EEPROM_PAGE_SIZE/2);
    TEST_ASSERT_TRUE(Eepromd_Read(EEPROM_PAGE_SIZE*2-5, data, EEPROM_PAGE_SIZE/2));

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
    memset(data, 0x00, EEPROM_PAGE_SIZE*3);
    TEST_ASSERT_TRUE(Eepromd_Read(EEPROM_PAGE_SIZE*2-5, data, EEPROM_PAGE_SIZE*3));

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

/** @} */
