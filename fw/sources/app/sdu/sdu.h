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
 * @file    sdu/sdu.h
 * @brief   SDU logic
 *
 * @addtogroup app-sdu
 * @{
 */

#ifndef __SDU_SDU_H
#define __SDU_SDU_H

#include <types.h>

typedef enum {
    RACE_MODE_ARCADE,
    RACE_MODE_ACCELERATION,
    RACE_MODE_LONG_DISTANCE,
    RACE_MODE_DEBUG,
} race_mode_t;

/**
 * Update internal car state
 *
 * @param [in] speed_dms        Car speed in dm/s
 * @param [in] speed_avg_dms    Average speed since race start in dm/s
 * @param [in] mode             Current car mode
 */
extern void Sdu_SetCarState(uint16_t speed_dms, uint16_t speed_avg_dms,
        race_mode_t mode);

/**
 * Initialize SDU
 */
extern void Sdu_Init(void);

#endif

/** @} */
