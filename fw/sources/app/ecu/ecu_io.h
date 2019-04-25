/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

    This file is part of BUT pneumobil ECU.

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
 * @file    ecu_io.h
 * @brief   Processing inputs and outputs to ECU module
 *
 * @addtogroup app-ecu
 * @{
 */

#ifndef __ECU_IO_H
#define __ECU_IO_H

#include <types.h>
#include <car_defs.h>

/* Current sensing resistor for pressure valve current loop */
#define ECU_RSENS_OHM 150

/** Structure for ECU input pins */
typedef struct {
    bool endstop_front;
    bool endstop_back;
    bool throttle;
    bool brake;
    bool shifting;
    bool horn;
} ecu_inputs_t;

/* Valves state structure */
typedef struct {
    valve_state_t front1;
    valve_state_t front2;
    valve_state_t back1;
    valve_state_t back2;
} ecu_valves_t;

/**
 * Set front valves to inputs to piston
 *
 * @param [in] dual Use dual valves mode
 */
extern void ECU_ValvesFrontIn(bool dual);

/**
 * Set front valves to exhausts
 *
 * @param [in] dual Use dual valves mode
 */
extern void ECU_ValvesFrontOut(bool dual);

/**
 * Set back valves to inputs
 *
 * @param [in] dual Use dual valves mode
 */
extern void ECU_ValvesBackIn(bool dual);

/**
 * Set back valves to exhausts
 *
 * @param [in] dual Use dual valves mode
 */
extern void ECU_ValvesBackOut(bool dual);

/**
 * Close back valves
 */
extern void ECU_ValvesBackClose(void);

/**
 * Close front valves
 */
extern void ECU_ValvesFrontClose(void);

/**
 * Close all valves
 */
extern void ECU_ValvesClose(void);

/**
 * Set valves to forward movement
 *
 * @param [in] dual Use dual valves mode
 */
extern void ECU_ValvesMoveFront(bool dual);

/**
 * Set valves to backward movement
 *
 * @param [in] dual Use dual valves mode
 */
extern void ECU_ValvesMoveBack(bool dual);

/**
 * Set valves to required state
 *
 * @param [in] valves   Valves state
 */
extern void ECU_ValvesSet(const ecu_valves_t *valves);

/**
 * Get current valves state
 *
 * @param [out] valves   Valves state
 */
extern void ECU_ValvesGet(ecu_valves_t *valves);

/**
 * Set pressure regulator output pressure
 *
 * @param [in] pressure_hpa Pressure in hPa to set
 */
extern void ECU_SetRegulatorPressure(uint32_t pressure_hpa);

/**
 * Read raw state of inputs to ECU
 *
 * @param [out] inputs Input states
 */
extern void ECU_GetRawInputs(ecu_inputs_t *inputs);

/**
 * Get currently shifted gear
 *
 * @return 0 for neutral, 1 and 2 for shifted gears
 */
extern uint8_t ECU_GetGear(void);

/**
 * Read inputs to ECU, reading serves as debouncing cycle
 *
 * @param [out] inputs Input states
 */
extern void ECU_GetInputs(ecu_inputs_t *inputs);

/**
 * Sound horn
 *
 * @param [in] on   If true, will force set the horn, if false, state depends
 *                  on button
 */
extern void ECU_SetHorn(bool on);
/**
 * Turn on the brake light
 *
 * @param [in] on   If true, will force set the light, if false, state depends
 *                  on peddal
 */
extern void ECU_SetBrakeLight(bool on);

/**
 * Control output 1
 */
extern void ECU_SetOut1(bool on);

/**
 * Control output 2
 */
extern void ECU_SetOut2(bool on);

/**
 * Initialize ECU IO module
 */
extern void ECU_IOInit(void);

#endif

/** @} */
