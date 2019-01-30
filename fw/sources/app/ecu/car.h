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
 * @file    ecu/car.h
 * @brief   Car data acquisition
 *
 * @addtogroup app-ecu
 * @{
 */

#ifndef __APP_ECU_CAR_H
#define __APP_ECU_CAR_H

#include <types.h>

/** Car state data */
typedef struct {
    uint16_t voltage24_mv;   /** 24 V branch voltage in mV */
    uint16_t voltage12_mv;   /** 12 V branch voltage in mV */
    uint16_t voltage3p3_mv;  /** 3.3 V branch voltage in mV */
    int16_t sys_temp_deg;    /** System temperature in degrees */

    int32_t speed_dms;       /** Current car speed in dm/s */
    uint32_t distance_m;    /** Distance traveled in meters */
    uint16_t pressure_kpa[6]; /** Pressures in kPa */
} car_data_t;

/**
 * Get current car state
 *
 * @param [out] state   Structure to store state to
 */
extern void Car_GetState(car_data_t *state);

/**
 * Initialize car state reading
 */
extern void Car_Init(void);

#endif

/** @} */
