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
 * @file    hmi/state.c
 * @brief   Collecting data from various modules to get car state
 *
 * @addtogroup app-hmi
 * @{
 */

#include <string.h>
#include "state.h"

static state_t statei_state;
static state_pneu_t * const statei_pneu = &statei_state.pneu;
static state_car_t * const statei_car = &statei_state.car;
static state_psu_t * const statei_psu = &statei_state.psu;

void State_UpdateCarState(uint16_t speed_dms, uint16_t speed_avg_dms,
        uint16_t distance_m, state_race_mode_t mode)
{
    statei_car->speed_kmh = speed_dms*0.36;
    statei_car->speed_avg_kmh = speed_avg_dms*0.36;
    statei_car->distance_m = distance_m;
    statei_car->mode = mode;
}

void State_UpdateCarIoState(state_valve_t front1, state_valve_t front2,
        state_valve_t back1, state_valve_t back2, bool endstop_front,
        bool endstop_back, bool throttle, bool brake, bool horn, bool shifting,
        uint8_t gear)
{
    statei_pneu->valve_front1 = front1;
    statei_pneu->valve_front2 = front2;
    statei_pneu->valve_back1 = back1;
    statei_pneu->valve_back2 = back2;
    statei_pneu->endstop_front = endstop_front;
    statei_pneu->endstop_back = endstop_back;
    statei_car->throttle = throttle;
    statei_car->brake = brake;
    statei_car->horn = horn;
    statei_car->shifting = shifting;
    statei_car->gear = gear;
}

void State_UpdatePneuState(uint16_t press1_kpa, uint16_t press2_kpa,
        uint16_t press3_kpa, uint8_t piston_pos_pct)
{
    statei_pneu->press1_kpa = press1_kpa;
    statei_pneu->press2_kpa = press2_kpa;
    statei_pneu->press3_kpa = press3_kpa;
    statei_pneu->piston_pct = piston_pos_pct;
}

void State_UpdateBattery(uint16_t bat1_mv, uint16_t bat2_mv, uint16_t cur_ma,
        uint8_t charge_pct)
{
    statei_psu->battery.bat1_mv = bat1_mv;
    statei_psu->battery.bat2_mv = bat2_mv;
    statei_psu->battery.cur_ma = cur_ma;
    statei_psu->battery.charge_pct = charge_pct;
}

void State_UpdatePSUCurrent(uint16_t cur5v_ma, uint16_t cur12v_ma,
        uint16_t cur24v_ma)
{
    statei_psu->psu5_ma = cur5v_ma;
    statei_psu->psu12_ma = cur12v_ma;
    statei_psu->psu24_ma = cur24v_ma;
}

void State_UpdatePSUVoltage(uint16_t volt5_mv, uint16_t volt12_mv,
        uint16_t volt24_mv)
{
    statei_psu->psu5_mv = volt5_mv;
    statei_psu->psu12_mv = volt12_mv;
    statei_psu->psu24_mv = volt24_mv;
}

state_t *State_Get(void)
{
    return &statei_state;
}

state_race_mode_t State_GetRaceMode(void)
{
    return statei_state.car.mode;
}

void State_Init(void)
{
    memset(&statei_state, 0, sizeof(statei_state));

    //TODO start thread that will be sending keep aplive to other nodes to see their state
}



/** @} */
