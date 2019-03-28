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
 * @file    ecu_io.c
 * @brief   Processing inputs and outputs to ECU module
 *
 *TODO add config to invert pin polarity
 *
 * @addtogroup app-ecu
 * @{
 */

#include <hal.h>

#include <utils/assert.h>
#include <modules/log.h>
#include <modules/config.h>
#include <utils/buttons.h>
#include <drivers/dac.h>

#include "race.h"
#include "ecu_io.h"

static bool ECUi_GetBtThrottle(void)
{
    static bool prev = 0;
    static bt_debounce_t deb = { 0 };
    bool state;

    state = Bt_Debounce(palReadLine(LINE_BT_SHIFTING), &deb);
    if (Bt_GetEdge(state, &prev) == BT_EDGE_RISING) {
        return true;
    }
    return false;
}

static bool ECUi_GetBtMode(void)
{
    static bool prev = 0;
    static bt_debounce_t deb = { 0 };
    bool state;

    state = Bt_Debounce(palReadLine(LINE_BT_MODE), &deb);
    if (Bt_GetEdge(state, &prev) == BT_EDGE_RISING) {
        return true;
    }
    return false;
}

void ECU_ValvesFrontIn(bool dual)
{
    palSetLine(LINE_VALVE_F1A);
    palClearLine(LINE_VALVE_F1B);

    if (dual) {
        palSetLine(LINE_VALVE_F2A);
    } else {
        palClearLine(LINE_VALVE_F2A);
    }
    palClearLine(LINE_VALVE_F2B);
}

void ECU_ValvesFrontOut(bool dual)
{
    palClearLine(LINE_VALVE_F1A);
    palSetLine(LINE_VALVE_F1B);

    if (dual) {
        palSetLine(LINE_VALVE_F2B);
    } else {
        palClearLine(LINE_VALVE_F2B);
    }
    palClearLine(LINE_VALVE_F2A);
}

void ECU_ValvesBackIn(bool dual)
{
    palSetLine(LINE_VALVE_B1A);
    palClearLine(LINE_VALVE_B1B);

    if (dual) {
        palSetLine(LINE_VALVE_B2A);
    } else {
        palClearLine(LINE_VALVE_B2A);
    }
    palClearLine(LINE_VALVE_B2B);
}

void ECU_ValvesBackOut(bool dual)
{
    palClearLine(LINE_VALVE_B1A);
    palSetLine(LINE_VALVE_B1B);

    if (dual) {
        palSetLine(LINE_VALVE_B2B);
    } else {
        palClearLine(LINE_VALVE_B2B);
    }
    palClearLine(LINE_VALVE_B2A);
}

void ECU_ValvesBackClose(void)
{
    palClearLine(LINE_VALVE_B1A);
    palClearLine(LINE_VALVE_B1B);
    palClearLine(LINE_VALVE_B2A);
    palClearLine(LINE_VALVE_B2B);
}

void ECU_ValvesFrontClose(void)
{
    palClearLine(LINE_VALVE_F1A);
    palClearLine(LINE_VALVE_F1B);
    palClearLine(LINE_VALVE_F2A);
    palClearLine(LINE_VALVE_F2B);
}

void ECU_ValvesClose(void)
{
    ECU_ValvesBackClose();
    ECU_ValvesFrontClose();
}

void ECU_ValvesMoveFront(bool dual)
{
    ECU_ValvesFrontOut(dual);
    ECU_ValvesBackIn(dual);
}

void ECU_ValvesMoveBack(bool dual)
{
    ECU_ValvesFrontIn(dual);
    ECU_ValvesBackOut(dual);
}

void ECU_ValvesSet(const ecu_valves_t *valves)
{
    ASSERT_NOT(valves == NULL);

    palWriteLine(LINE_VALVE_F1A, valves->front1 & 0x01);
    palWriteLine(LINE_VALVE_F1B, (valves->front1 >> 1) & 0x01);
    palWriteLine(LINE_VALVE_F2A, valves->front2 & 0x01);
    palWriteLine(LINE_VALVE_F2B, (valves->front2 >> 1) & 0x01);

    palWriteLine(LINE_VALVE_B1A, valves->back1 & 0x01);
    palWriteLine(LINE_VALVE_B1B, (valves->back1 >> 1) & 0x01);
    palWriteLine(LINE_VALVE_B2A, valves->back2 & 0x01);
    palWriteLine(LINE_VALVE_B2B, (valves->back2 >> 1) & 0x01);
}

void ECU_ValvesGet(ecu_valves_t *valves)
{
    ASSERT_NOT(valves == NULL);

    valves->front1 = palReadLine(LINE_VALVE_F1A) | (palReadLine(LINE_VALVE_F1B) << 1);
    if (valves->front1 > 2) {
        valves->front1 = VALVE_CLOSED;
    }

    valves->back1 = palReadLine(LINE_VALVE_B1A) | (palReadLine(LINE_VALVE_B1B) << 1);
    if (valves->back1 > 2) {
        valves->back1 = VALVE_CLOSED;
    }

    valves->front2 = palReadLine(LINE_VALVE_F2A) | (palReadLine(LINE_VALVE_F2B) << 1);
    if (valves->front2 > 2) {
        valves->front2 = VALVE_CLOSED;
    }

    valves->back2 = palReadLine(LINE_VALVE_B2A) | (palReadLine(LINE_VALVE_B2B) << 1);
    if (valves->back2 > 2) {
        valves->back2 = VALVE_CLOSED;
    }
}

void ECU_SetRegulatorPressure(uint32_t pressure_hpa)
{
    uint16_t voltage_mv;
    /*
     * 4mA = 0Bar, 20mA = 10Bar
     * Measuring voltage drop on sense resistor (150 ohm)
     *
     * p = 10000/16*I - 10000/4 = 625*I - 2500
     * Where p is pressure in hpa, I current in mA
     *
     * Voltage on resistor is:
     * U = RI = R * (p+2500)/625
     */
    if (pressure_hpa > 10000)
            pressure_hpa = 10000;

    voltage_mv = (ECU_RSENS_OHM * (pressure_hpa + 2500))/625;
    Dacd_SetOutputMv(voltage_mv);
}

void ECU_GetRawInputs(ecu_inputs_t *inputs)
{
    ASSERT_NOT(inputs == NULL);

    if (Config_GetBool(CONFIG_BOOL_ENCODER_INVERT)) {
        inputs->endstop_front = !palReadLine(LINE_ENDSTOP_F1);
        inputs->endstop_back = !palReadLine(LINE_ENDSTOP_B1);
    } else {
        inputs->endstop_front = palReadLine(LINE_ENDSTOP_F1);
        inputs->endstop_back = palReadLine(LINE_ENDSTOP_B1);
    }
    inputs->throttle = !palReadLine(LINE_THROTTLE);
    inputs->brake = !palReadLine(LINE_BRAKE);
    inputs->shifting = palReadLine(LINE_BT_SHIFTING);
    inputs->horn = !palReadLine(LINE_BT_HORN);
}

uint8_t ECU_GetGear(void)
{
    bool gear1 = !palReadLine(LINE_GEARBOX_1);
    bool gear2 = !palReadLine(LINE_GEARBOX_2);

    if (!gear1 && !gear2) {
        return 0;
    }

    if (gear1) {
        return 1;
    }
    return 2;
}

void ECU_GetInputs(ecu_inputs_t *inputs)
{
    static bt_debounce_t deb[6] = { 0 };
    static bool prev[2] = { 0 };
    ASSERT_NOT(inputs == NULL);

    ECU_GetRawInputs(inputs);
    inputs->endstop_front = Bt_Debounce(inputs->endstop_front, &deb[0]);
    inputs->endstop_front = Bt_GetEdge(inputs->endstop_front, &prev[0]) == BT_EDGE_RISING;
    inputs->endstop_back = Bt_Debounce(inputs->endstop_back, &deb[1]);
    inputs->endstop_back = Bt_GetEdge(inputs->endstop_back, &prev[1]) == BT_EDGE_RISING;

    inputs->throttle = Bt_Debounce(inputs->throttle, &deb[2]) | ECUi_GetBtThrottle();
    inputs->brake = Bt_Debounce(inputs->brake, &deb[3]);
    inputs->shifting = Bt_Debounce(inputs->shifting, &deb[4]);
    inputs->horn = Bt_Debounce(inputs->horn, &deb[5]);

    if (ECUi_GetBtMode()) {
        Race_NextMode();
    }
}

void ECU_SetHorn(bool on)
{
    if (on) {
        palSetLineMode(LINE_BT_HORN, PAL_MODE_OUTPUT_PUSHPULL);
        palSetLine(LINE_BT_HORN);
    } else {
        palSetLineMode(LINE_BT_HORN, PAL_MODE_INPUT);
    }
}

void ECU_SetBrakeLight(bool on)
{
    if (on) {
        palSetLineMode(LINE_BRAKE, PAL_MODE_OUTPUT_PUSHPULL);
        palSetLine(LINE_BRAKE);
    } else {
        palSetLineMode(LINE_BRAKE, PAL_MODE_INPUT);
    }

}

void ECU_SetOut1(bool on)
{
    palWriteLine(LINE_OUT1, (uint8_t)on);
}

void ECU_SetOut2(bool on)
{
    palWriteLine(LINE_OUT1, (uint8_t)on);
}

void ECU_IOInit(void)
{
    Dacd_Init();
}

/** @} */
