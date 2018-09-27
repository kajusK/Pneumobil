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
 * @file    test_crc.c
 * @brief   Unit test for CRC calculations
 *
 * @addtogroup tests
 * @{
 */

#include <main.h>
#include "utils/crc.c"

/* *****************************************************************************
 * Mocks
***************************************************************************** */

/* *****************************************************************************
 * Tests
***************************************************************************** */
TEST_GROUP(CRC);

TEST_SETUP(CRC)
{
}

TEST_TEAR_DOWN(CRC)
{

}

TEST(CRC, CRC8Add)
{
    TEST_ASSERT_EQUAL(0x00, CRC8_Add(0x00, CRC8_INITIAL_VALUE));
    TEST_ASSERT_EQUAL(0x58, CRC8_Add(0xab, CRC8_INITIAL_VALUE));
    TEST_ASSERT_EQUAL(0xf3, CRC8_Add(0xff, CRC8_INITIAL_VALUE));
}

TEST(CRC, CRC8)
{
    uint8_t buf[10] = {0x0a, 0x13, 0x15, 0xff, 0x11, 0xa8, 0xfa, 0xac, 0xcd, 0x00};

    TEST_ASSERT_EQUAL(0xcf, CRC8(buf, 10));
}

TEST_GROUP_RUNNER(CRC)
{
    RUN_TEST_CASE(CRC, CRC8Add);
    RUN_TEST_CASE(CRC, CRC8);
}

void CRC_RunTests(void)
{
    RUN_TEST_GROUP(CRC);
}
