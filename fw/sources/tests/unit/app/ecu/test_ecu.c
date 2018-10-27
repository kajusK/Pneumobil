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
 * @file    test_ecu.c
 * @brief   Unit test for engine control
 *
 * @addtogroup tests
 * @{
 */

#include <main.h>
#include "app/ecu/ecu.c"

static enum {
    VALVES_CLOSED,
    VALVES_MOVE_BACK,
    VALVES_MOVE_FRONT,
    VALVES_BRAKE_BACK,
    VALVES_BRAKE_FRONT,
} valves;

static int16_t encoder_get;
static bool encoder_set_called;
static int16_t encoder_set;
static uint32_t piston_len;
static uint32_t piston_len_set;
static ecu_control_t control;
static bool encoder_inverted;

/* *****************************************************************************
 * Mocks
***************************************************************************** */
static void Log_Info(log_src_t src, const char *format, ...)
{
    (void) src;
    (void) format;
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

static void Log_Error(log_src_t src, const char *format, ...)
{
    (void) src;
    (void) format;
}

static bool Config_GetBool(config_item_bool_t item)
{
    (void) item;
    return false;
}

static void Config_SetBool(config_item_bool_t item, bool value)
{
    (void) item;
    (void) value;
}

static uint32_t Config_GetUint(config_item_uint_t item)
{
    switch (item) {
        case CONFIG_UINT_PISTON_LEN:
            return piston_len;
            break;
        default:
            TEST_FAIL_MESSAGE("Unexpected item read");
            break;
    }
    return 0;
}

static void Config_SetUint(config_item_uint_t item, uint32_t value)
{
    switch (item) {
        case CONFIG_UINT_PISTON_LEN:
            piston_len_set = value;
            break;
        default:
            TEST_FAIL_MESSAGE("Unexpected item changed");
            break;
    }
}

static void Encoderd_InvertDirection(void)
{
    encoder_inverted = true;
}

static void Encoderd_Set(int16_t value)
{
    encoder_set = value;
    encoder_set_called = true;
}

static int16_t Encoderd_Get(void)
{
    return encoder_get;
}

static void Encoderd_Init(void)
{

}

static void ECU_IOInit(void)
{

}

static void ECU_GetInputs(ecu_inputs_t *inputs)
{
    (void) inputs;
}

static void ECU_ValvesClose(void)
{
    valves = VALVES_CLOSED;
}

static void ECU_ValvesMoveFront(bool dual)
{
    (void) dual;
    valves = VALVES_MOVE_FRONT;
}

static void ECU_ValvesMoveBack(bool dual)
{
    (void) dual;
    valves = VALVES_MOVE_BACK;
}

static void ECU_ValvesBackOut(bool dual)
{
    (void) dual;
    valves = VALVES_BRAKE_BACK;
}

static void ECU_ValvesFrontOut(bool dual)
{
    (void) dual;
    valves = VALVES_BRAKE_FRONT;
}

static void ECU_ValvesBackClose(void)
{
    valves = VALVES_BRAKE_FRONT;
}

static void ECU_ValvesFrontClose(void)
{
    valves = VALVES_BRAKE_BACK;
}

/* *****************************************************************************
 * Tests
***************************************************************************** */

TEST_GROUP(ECU);

TEST_SETUP(ECU)
{
    encoder_set = -1;
    encoder_get = -1;
    encoder_inverted = false;
    encoder_set_called = false;
    piston_len = 0;
    piston_len_set = 0;
    valves = VALVES_CLOSED;

    control.inputs.endstop_back = false;
    control.inputs.endstop_front = false;
    control.inputs.throttle = false;
    control.inputs.brake = false;
    control.inputs.shifting = false;
    control.state = ECU_STATE_INIT;
    control.dir = ECU_DIR_BACK;
}

TEST_TEAR_DOWN(ECU)
{

}

TEST(ECU, EdgesInitNoConfig)
{
    assert_should_fail = true;
    ECUi_EdgesInit(NULL);
    assert_should_fail = false;

    /* Search back endstop */
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_CLOSED, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_INIT, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    control.inputs.throttle = true;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_MOVE_BACK, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_INIT, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    /* Test throttle release */
    control.inputs.throttle = false;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_CLOSED, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_INIT, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    /* Back endstop found */
    control.inputs.throttle = true;
    control.inputs.endstop_back = true;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_MOVE_FRONT, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_INIT, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);
    TEST_ASSERT_EQUAL(0, encoder_set);
    TEST_ASSERT_TRUE(encoder_set_called);

    /* Front endstop found */
    control.inputs.endstop_back = false;
    control.inputs.endstop_front = true;
    encoder_get = 1024;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_CLOSED, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_IDLE, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);
    TEST_ASSERT_EQUAL(1024, piston_len_set);
    TEST_ASSERT_FALSE(encoder_inverted);

    /* Inverted encoder */
    control.state = ECU_STATE_INIT;
    encoder_get = -1024;
    piston_len_set = 0;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_CLOSED, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_IDLE, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);
    TEST_ASSERT_EQUAL(1024, piston_len_set);
    TEST_ASSERT_EQUAL(1024, encoder_set);
    TEST_ASSERT_TRUE(encoder_inverted);


    /* Init with front encoder hit first, move to back and then to front, simplest */
    control.inputs.endstop_front = true;
    control.state = ECU_STATE_INIT;
    control.dir = ECU_DIR_BACK;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_MOVE_BACK, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_INIT, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    control.inputs.endstop_back = true;
    control.inputs.endstop_front = false;
    encoder_set_called = false;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_MOVE_FRONT, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_INIT, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);
    TEST_ASSERT_TRUE(encoder_set_called);
    TEST_ASSERT_EQUAL(0, encoder_set);
}

TEST(ECU, EdgesInitWithConfig)
{
    piston_len = 1024;

    /* Search back endstop */
    control.inputs.throttle = true;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_MOVE_BACK, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_INIT, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    /* Back endstop hit */
    control.inputs.endstop_back = true;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_CLOSED, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_IDLE, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);
    TEST_ASSERT_TRUE(encoder_set_called);
    TEST_ASSERT_EQUAL(0, encoder_set);

    /* front endstop hit */
    control.inputs.endstop_front = true;
    control.inputs.endstop_back = false;
    control.state = ECU_STATE_INIT;
    encoder_set_called = false;
    ECUi_EdgesInit(&control);
    TEST_ASSERT_EQUAL(VALVES_CLOSED, valves);
    TEST_ASSERT_EQUAL(ECU_STATE_IDLE, control.state);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);
    TEST_ASSERT_TRUE(encoder_set_called);
    TEST_ASSERT_EQUAL(1024, encoder_set);
}

TEST(ECU, GetPistonPos)
{
    assert_should_fail = true;
    ECUi_GetPistonPosPct(NULL);
    assert_should_fail = false;

    encoder_get = 1024;
    piston_len = 1024;

    /* Check correct position while encoder in range */
    TEST_ASSERT_EQUAL(100, ECUi_GetPistonPosPct(&control.inputs));
    TEST_ASSERT_FALSE(encoder_set_called);

    encoder_get = 0;
    TEST_ASSERT_EQUAL(0, ECUi_GetPistonPosPct(&control.inputs));
    TEST_ASSERT_FALSE(encoder_set_called);

    encoder_get = 512;
    TEST_ASSERT_EQUAL(50, ECUi_GetPistonPosPct(&control.inputs));
    TEST_ASSERT_FALSE(encoder_set_called);

    /* Test correction of overflows */
    encoder_get = -10;
    TEST_ASSERT_EQUAL(0, ECUi_GetPistonPosPct(&control.inputs));
    TEST_ASSERT_TRUE(encoder_set_called);
    TEST_ASSERT_EQUAL(0, encoder_set);

    encoder_set_called = false;
    encoder_get = 2048;
    TEST_ASSERT_EQUAL(100, ECUi_GetPistonPosPct(&control.inputs));
    TEST_ASSERT_TRUE(encoder_set_called);
    TEST_ASSERT_EQUAL(1024, encoder_set);

    /* Test correction of position for endstop hit */
    control.inputs.endstop_back = true;
    encoder_set_called = false;
    encoder_get = 0;
    TEST_ASSERT_EQUAL(0, ECUi_GetPistonPosPct(&control.inputs));
    TEST_ASSERT_FALSE(encoder_set_called);

    encoder_get = 10;
    TEST_ASSERT_EQUAL(0, ECUi_GetPistonPosPct(&control.inputs));
    TEST_ASSERT_TRUE(encoder_set_called);
    TEST_ASSERT_EQUAL(0, encoder_set);

    /* Test correction of position for endstop second hit */
    control.inputs.endstop_back = false;
    control.inputs.endstop_front = true;
    encoder_set_called = false;
    encoder_get = 1024;
    TEST_ASSERT_EQUAL(100, ECUi_GetPistonPosPct(&control.inputs));
    TEST_ASSERT_FALSE(encoder_set_called);

    encoder_get = 820;
    TEST_ASSERT_EQUAL(100, ECUi_GetPistonPosPct(&control.inputs));
    TEST_ASSERT_TRUE(encoder_set_called);
    TEST_ASSERT_EQUAL(1024, encoder_set);
}

TEST(ECU, RunningNormal)
{
    piston_len = 1024;
    control.state = ECU_STATE_IDLE;
    control.filling_pct = 50;
    control.piston_start_pct = 0;
    control.deadtime_ms = 1;
    encoder_get = 128;

    /* Throttle released, idle */
    control.inputs.throttle = false;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_IDLE, control.state);
    TEST_ASSERT_EQUAL(VALVES_CLOSED, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    /* Below filling */
    control.inputs.throttle = true;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_MOVE, control.state);
    TEST_ASSERT_EQUAL(VALVES_MOVE_FRONT, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);

    /* above filling, start braking */
    encoder_get = 800;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_DEADPOINT, control.state);
    TEST_ASSERT_EQUAL(VALVES_BRAKE_FRONT, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);

    /* should timeout immediately in test environment */
    encoder_get = 1000;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_MOVE, control.state);
    TEST_ASSERT_EQUAL(VALVES_MOVE_BACK, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    /* without throttle it should wait for edge and go to idle */
    control.inputs.throttle = false;
    encoder_get = 700;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_MOVE, control.state);
    TEST_ASSERT_EQUAL(VALVES_MOVE_BACK, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    encoder_get = 50;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_DEADPOINT, control.state);
    TEST_ASSERT_EQUAL(VALVES_BRAKE_BACK, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_IDLE, control.state);
    TEST_ASSERT_EQUAL(VALVES_CLOSED, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);
}

TEST(ECU, RunningNoDeadtime)
{
    piston_len = 1024;
    control.state = ECU_STATE_IDLE;
    control.filling_pct = 50;
    control.piston_start_pct = 0;
    control.deadtime_ms = 0;
    encoder_get = 128;

    /* Below filling */
    control.inputs.throttle = true;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_MOVE, control.state);
    TEST_ASSERT_EQUAL(VALVES_MOVE_FRONT, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);

    /* above filling, reverse immediately */
    encoder_get = 800;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_MOVE, control.state);
    TEST_ASSERT_EQUAL(VALVES_MOVE_BACK, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    encoder_get = 1000;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_MOVE, control.state);
    TEST_ASSERT_EQUAL(VALVES_MOVE_BACK, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_BACK, control.dir);

    /* and reverse again */
    encoder_get = 50;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_MOVE, control.state);
    TEST_ASSERT_EQUAL(VALVES_MOVE_FRONT, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);

    /* without throttle it should wait for edge and go to idle */
    control.inputs.throttle = false;
    encoder_get = 200;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_MOVE, control.state);
    TEST_ASSERT_EQUAL(VALVES_MOVE_FRONT, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);

    encoder_get = 1000;
    ECUi_PneuStep(&control);
    TEST_ASSERT_EQUAL(ECU_STATE_IDLE, control.state);
    TEST_ASSERT_EQUAL(VALVES_CLOSED, valves);
    TEST_ASSERT_EQUAL(ECU_DIR_FRONT, control.dir);
}

TEST_GROUP_RUNNER(ECU)
{
    RUN_TEST_CASE(ECU, EdgesInitNoConfig);
    RUN_TEST_CASE(ECU, EdgesInitWithConfig);
    RUN_TEST_CASE(ECU, GetPistonPos);
    RUN_TEST_CASE(ECU, RunningNormal);
    RUN_TEST_CASE(ECU, RunningNoDeadtime);
}

void ECU_RunTests(void)
{
    RUN_TEST_GROUP(ECU);
}
