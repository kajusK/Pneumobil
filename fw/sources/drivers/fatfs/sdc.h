
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
 * @file    adc.c
 * @brief   ADC driver
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __DRIVERS_SDC_H
#define __DRIVERS_SDC_H

#include <types.h>

typedef void (*sdcd_cb_t)(void);

/**
 * Add callbac to be called on card insertion
 *
 * @param [in] cb       Callback
 * @return True if ok, false if all possible callback are occupied
 */
extern bool SDCd_AddInsertCallback(sdcd_cb_t cb);

/**
 * Check if the sd card is mounted and ready to be written
 *
 * @return True if ready
 */
extern bool SDCd_IsReady(void);

/**
 * Initialize SDC driver and related fatfs
 */
extern void SDCd_Init(void);

#endif

/** @} */
