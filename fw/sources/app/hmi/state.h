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
 * @file    hmi/state.c
 * @brief   Collecting data from various modules to get car state
 *
 * @addtogroup app-hmi
 */

#ifndef __MODULES_STATE_H
#define __MODULES_STATE_H

#include <types.h>

typedef enum {
    VALVE_CLOSED,
    VALVE_IN,
    VALVE_OUT,
} state_valve_t;

typedef enum {
    RACE_MODE_ARCADE,
    RACE_MODE_ACCELERATION,
    RACE_MODE_LONG_DISTANCE,
    RACE_MODE_DEBUG,
} state_race_mode_t;

typedef struct {
    uint8_t piston_pct;
    uint16_t press1_kpa;
    uint16_t press2_kpa;
    uint16_t press3_kpa;
    state_valve_t valve_front1;
    state_valve_t valve_front2;
    state_valve_t valve_back1;
    state_valve_t valve_back2;
    bool endstop_front;
    bool endstop_back;
} state_pneu_t;

typedef struct {
    float speed_kmh;
    float speed_avg_kmh;
    uint16_t distance_m;
    state_race_mode_t mode;
    bool throttle;
    bool brake;
    bool horn;
    bool shifting;
    uint8_t gear;
} state_car_t;

typedef struct {
    uint16_t bat1_mv;
    uint16_t bat2_mv;
    uint16_t cur_ma;
    uint8_t charge_pct;
} state_battery_t;

typedef struct {
    state_battery_t battery;
    uint16_t psu5_mv;
    uint16_t psu12_mv;
    uint16_t psu24_mv;
    uint16_t psu5_ma;
    uint16_t psu12_ma;
    uint16_t psu24_ma;
} state_psu_t;

typedef struct {
    uint32_t _seen;
    bool online;
    uint16_t core_mv;
    int8_t temp_c;
    uint16_t uptime_s;
} state_node_t;

typedef struct {
    state_node_t psu;
    state_node_t hmi;
    state_node_t ecu;
    state_node_t sdu;
    state_node_t dbg;
} state_nodes_t;

typedef struct {
    state_psu_t psu;
    state_car_t car;
    state_pneu_t pneu;
    state_nodes_t node;
} state_t;

/*
 * Setters for the internal state variables, usually called from comm module
 */
extern void State_UpdateCarState(uint16_t speed_dms, uint16_t speed_avg_dms,
        uint16_t distance_m, state_race_mode_t mode);
extern void State_UpdateCarIoState(state_valve_t front1, state_valve_t front2,
        state_valve_t back1, state_valve_t back2, bool endstop_front,
        bool endstop_back, bool throttle, bool brake, bool horn, bool shifting,
        uint8_t gear);
extern void State_UpdatePneuState(uint16_t press1_kpa, uint16_t press2_kpa,
        uint16_t press3_kpa, uint8_t piston_pos_pct);
extern void State_UpdateBattery(uint16_t bat1_mv, uint16_t bat2_mv,
        uint16_t cur_ma, uint8_t charge_pct);
extern void State_UpdatePSUCurrent(uint16_t cur5v_ma, uint16_t cur12v_ma,
        uint16_t cur24v_ma);
extern void State_UpdatePSUVoltage(uint16_t volt5_mv, uint16_t volt12_mv,
        uint16_t volt24_mv);

#include <modules/comm/comm.h>
extern void State_UpdateNode(comm_node_t node, uint16_t uptime_s, int8_t temp_c,
        uint16_t core_mv);

/**
 * Get car state data
 *
 * @return Pointer to internal status structure
 */
extern state_t *State_Get(void);

/**
 * Get current ECU race mode
 */
extern state_race_mode_t State_GetRaceMode(void);

/**
 * Send command to change race mode to ecu
 *
 * @param [in] mode     Mode to be used
 */
extern bool State_SetRaceMode(state_race_mode_t mode);

/**
 * Initialize State module
 */
extern void State_Init(void);

#endif

/** @} */
