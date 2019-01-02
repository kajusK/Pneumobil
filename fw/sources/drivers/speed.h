
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
 * @file    speed.c
 * @brief   Speed and distance sensor
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __DRIVERS_SPEED_H
#define __DRIVERS_SPEED_H

#include <types.h>

/**
 * Time period in ms during which pulses are counted
 *
 * Higher value means higher accuracy, but slower speed updating
 */
#define SPEEDD_MEAS_PERIOD_MS 250

/**
 * Reset the distance counter
 */
extern void Speedd_Reset(void);

/**
 * Get distance travelled since last restart/init
 *
 * @return Distance traveled in meters
 */
extern uint32_t Speedd_GetDistanceM(void);

/**
 * Get car speed in dm/s (to avoid float numbers)
 *
 * @return Speed in dm/s
 */
extern uint32_t Speedd_GetSpeedDms(void);

/**
 * Initialize the speed driver
 *
 * @param [in] mm_per_pulse Distance in mm traveled between two sensor pulses
 */
extern void Speedd_Init(uint16_t mm_per_pulse);

#endif

/** @} */
