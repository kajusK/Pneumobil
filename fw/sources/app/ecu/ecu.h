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
 * @file    ecu.h
 * @brief   Engine control block - timing of valves, etc
 *
 * @addtogroup app-ecu
 * @{
 */

#ifndef __APP_ECU_H
#define __APP_ECU_H

#include <types.h>

/** Race mode selection */
typedef enum {
    RACE_MODE_ARCADE,
    RACE_MODE_ACCELERATION,
    RACE_MODE_LONG_DISTANCE,
    RACE_MODE_DEBUG,            /** ECU logic is paused in DEBUG mode */
} ecu_race_mode_t;

/**
 * Get current ECU mode
 *
 * @return ECU mode
 */
extern ecu_race_mode_t ECU_GetMode(void);

/**
 * Get piston position without any corrections
 *
 * @return Position in percents
 */
extern uint8_t ECU_GetRawPistonPosPct(void);

/**
 * Initialize engine control unit
 */
extern void ECU_Init(void);

#endif

/** @} */
