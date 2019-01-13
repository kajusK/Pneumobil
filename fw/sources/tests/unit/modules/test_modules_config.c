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
 * @file    test_modules_config.c
 * @brief   Unit test for Configuration module
 *
 * @addtogroup tests
 * @{
 */
#include <string.h>

#include <main.h>
#include "modules/log.h"
#include "modules/config.c"

static bool update_called;
static storage_update_t st_update;

/* *****************************************************************************
 * Mocks
***************************************************************************** */
static void Log_Debug(log_src_t src, const char *format, ...)
{
    (void) src;
    (void) format;
}

static void Storage_Update(storage_update_t update)
{
    update_called = true;
    st_update = update;
}

/* *****************************************************************************
 * Tests
***************************************************************************** */
TEST_GROUP(Config);

TEST_SETUP(Config)
{
    Config_Reset();
    update_called = false;
}

TEST_TEAR_DOWN(Config)
{

}

TEST(Config, SetGetUint)
{
    assert_should_fail = true;
    Config_SetUint(CONFIG_UINT_COUNT, 1);
    Config_SetUint(CONFIG_UINT_COUNT + 10, 1);
    Config_GetUint(CONFIG_UINT_COUNT);
    Config_GetUint(CONFIG_UINT_COUNT + 10);
    assert_should_fail = false;

    Config_SetUint(CONFIG_UINT_TEST1, 123);
    TEST_ASSERT_EQUAL(123, Config_GetUint(CONFIG_UINT_TEST1));
    TEST_ASSERT_TRUE(update_called);
    TEST_ASSERT_EQUAL(STORAGE_UPDATE_UINT, st_update);

    Config_SetUint(CONFIG_UINT_TEST2, 321);
    TEST_ASSERT_EQUAL(321, Config_GetUint(CONFIG_UINT_TEST2));

    Config_SetUint(CONFIG_UINT_COUNT - 1, 123456);
    TEST_ASSERT_EQUAL(123456, Config_GetUint(CONFIG_UINT_COUNT - 1));
}

TEST(Config, SetGetBool)
{
    assert_should_fail = true;
    Config_SetBool(CONFIG_BOOL_COUNT, true);
    Config_SetBool(CONFIG_BOOL_COUNT + 10, true);
    Config_GetBool(CONFIG_BOOL_COUNT);
    Config_GetBool(CONFIG_BOOL_COUNT + 10);
    assert_should_fail = false;

    /* write some bool data */
    for (int i = 0; i < CONFIG_BOOL_COUNT; i++) {
        Config_SetBool(i, i % 2);
        TEST_ASSERT_EQUAL(i % 2, Config_GetBool(i));
    }
    TEST_ASSERT_TRUE(update_called);
    TEST_ASSERT_EQUAL(STORAGE_UPDATE_BOOL, st_update);

    /* Verify no data were changed */
    for (int i = 0; i < CONFIG_BOOL_COUNT; i++) {
        TEST_ASSERT_EQUAL(i % 2, Config_GetBool(i));
    }
}

TEST(Config, ResetUint)
{
    Config_ResetUint();
    TEST_ASSERT_TRUE(update_called);
    TEST_ASSERT_EQUAL(STORAGE_UPDATE_UINT, st_update);

    TEST_ASSERT_EQUAL(1000, Config_GetUint(CONFIG_UINT_TEST1));
    TEST_ASSERT_EQUAL(2000, Config_GetUint(CONFIG_UINT_TEST2));
    TEST_ASSERT_EQUAL(3000, Config_GetUint(CONFIG_UINT_TEST3));
    TEST_ASSERT_EQUAL(4000, Config_GetUint(CONFIG_UINT_TEST4));
}

TEST(Config, ResetBool)
{
    Config_ResetBool();
    TEST_ASSERT_TRUE(update_called);
    TEST_ASSERT_EQUAL(STORAGE_UPDATE_BOOL, st_update);

    TEST_ASSERT_EQUAL(false, Config_GetBool(CONFIG_BOOL_TEST1));
    TEST_ASSERT_EQUAL(true, Config_GetBool(CONFIG_BOOL_TEST5));

    TEST_ASSERT_EQUAL(true, Config_GetBool(CONFIG_BOOL_TEST12));
    TEST_ASSERT_EQUAL(false, Config_GetBool(CONFIG_BOOL_TEST11));
    TEST_ASSERT_EQUAL(true, Config_GetBool(CONFIG_BOOL_TEST10));
    TEST_ASSERT_EQUAL(false, Config_GetBool(CONFIG_BOOL_TEST9));
}

TEST(Config, GetSystem)
{
    system_info_t *info;

    info = Config_GetSystem();
    TEST_ASSERT_EQUAL(5, info->major);
    TEST_ASSERT_EQUAL(9, info->minor);
    TEST_ASSERT_EQUAL_STRING("TESTS", info->name);
    TEST_ASSERT_EQUAL_STRING(__DATE__, info->build_on);
}

TEST_GROUP_RUNNER(Config)
{
    RUN_TEST_CASE(Config, SetGetUint);
    RUN_TEST_CASE(Config, SetGetBool);
    RUN_TEST_CASE(Config, ResetUint);
    RUN_TEST_CASE(Config, ResetBool);
    RUN_TEST_CASE(Config, GetSystem);
}

void Config_RunTests(void)
{
    RUN_TEST_GROUP(Config);
}

/** @} */
