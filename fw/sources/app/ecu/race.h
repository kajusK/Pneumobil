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
 * @file    ecu/race.c
 * @brief   Handling race modes, parameters changing,...
 *
 * @addtogroup app-ecu
 * @{
 */

#ifndef __APP_ECU_RACE_H
#define __APP_ECU_RACE_H

#include <types.h>

/** Race mode selection */
typedef enum {
    RACE_MODE_ARCADE,
    RACE_MODE_ACCELERATION,
    RACE_MODE_LONG_DISTANCE,
    RACE_MODE_DEBUG,            /** ECU logic is paused in DEBUG mode */
} race_mode_t;

/**
 * Get currently set filling percentage
 */
extern uint8_t  Race_GetFillingPct(void);

/**
 * Get currently set deadpoint time
 */
extern uint16_t Race_GetDeadpointMs(void);

/**
 * Get current ECU mode
 *
 * @return ECU mode
 */
extern race_mode_t Race_GetMode(void);

/**
 * Get time since race start
 *
 * @return Time since start in ms
 */
extern uint32_t Race_GetDurationMs(void);

/**
 * Move to next race mode (e.g. after button press)
 */
extern void Race_NextMode(void);

/**
 * (Re)Start race
 *
 * @param [in] mode     Mode to use for this race or debug
 * @return False if nonexistent mode selected
 */
extern bool Race_Start(race_mode_t mode);

#endif

/** @} */
