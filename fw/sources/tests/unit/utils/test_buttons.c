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
 * @file    test_buttons.c
 * @brief   Unit test for buttons handling
 *
 * @addtogroup tests
 * @{
 */

#include <main.h>
#include "utils/buttons.c"

/* *****************************************************************************
 * Mocks
***************************************************************************** */

/* *****************************************************************************
 * Tests
***************************************************************************** */
TEST_GROUP(Buttons);

TEST_SETUP(Buttons)
{
}

TEST_TEAR_DOWN(Buttons)
{

}

TEST(Buttons, Edge)
{
    bool prev = false;

    assert_should_fail = true;
    Bt_GetEdge(0, NULL);
    assert_should_fail = false;

    TEST_ASSERT_EQUAL(0, Bt_GetEdge(0, &prev));
    TEST_ASSERT_EQUAL(BT_EDGE_RISING, Bt_GetEdge(1, &prev));
    TEST_ASSERT_EQUAL(0, Bt_GetEdge(1, &prev));
    TEST_ASSERT_EQUAL(BT_EDGE_FALLING, Bt_GetEdge(0, &prev));
    TEST_ASSERT_EQUAL(0, Bt_GetEdge(0, &prev));
}

TEST(Buttons, Debounce)
{
    bt_debounce_t deb = { 0 };

    assert_should_fail = true;
    Bt_Debounce(0, NULL);
    assert_should_fail = false;

    TEST_ASSERT_TRUE(Bt_Debounce(1, &deb));
    /* 4 next cycles are ignored */
    TEST_ASSERT_TRUE(Bt_Debounce(1, &deb));
    TEST_ASSERT_TRUE(Bt_Debounce(1, &deb));
    TEST_ASSERT_TRUE(Bt_Debounce(0, &deb));
    TEST_ASSERT_TRUE(Bt_Debounce(1, &deb));
    /* Stable state */
    TEST_ASSERT_TRUE(Bt_Debounce(1, &deb));
    TEST_ASSERT_TRUE(Bt_Debounce(1, &deb));

    /* New state */
    TEST_ASSERT_FALSE(Bt_Debounce(0, &deb));

    /* 4 next cycles are ignored */
    TEST_ASSERT_FALSE(Bt_Debounce(0, &deb));
    TEST_ASSERT_FALSE(Bt_Debounce(1, &deb));
    TEST_ASSERT_FALSE(Bt_Debounce(0, &deb));
    TEST_ASSERT_FALSE(Bt_Debounce(1, &deb));

    /* Change just after debouncing */
    TEST_ASSERT_TRUE(Bt_Debounce(1, &deb));
}

TEST_GROUP_RUNNER(Buttons)
{
    RUN_TEST_CASE(Buttons, Edge);
    RUN_TEST_CASE(Buttons, Debounce);
}

void Buttons_RunTests(void)
{
    RUN_TEST_GROUP(Buttons);
}
