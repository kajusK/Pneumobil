/*
    BUT pneumobil - Copyright (C) 2019 Jakub Kaderka.

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
 * @file    car_defs.h
 * @brief   Definition of car priperties that must have same value in all apps
 *
 * @{
 */

#ifndef __CAR_DEFS_H
#define __CAR_DEFS_H

/** Race mode selection */
typedef enum {
    RACE_MODE_ARCADE,
    RACE_MODE_ACCELERATION,
    RACE_MODE_LONG_DISTANCE,
    RACE_MODE_DEBUG,            /** ECU logic is paused in DEBUG mode */
} race_mode_t;

/** Valve state */
typedef enum {
    VALVE_CLOSED = 0,
    VALVE_IN = 1,
    VALVE_OUT = 2,
} valve_state_t;

#endif

/** @} */
