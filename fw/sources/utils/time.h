
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
 * @file    time.h
 * @brief   Common time measurements (abstraction over chibios)
 *
 * @addtogroup utils
 * @{
 */

#ifndef __UTILS_TIME_H
#define __UTILS_TIME_H

#include <types.h>

/**
 * Get time since boot in milliseconds
 *
 * @return time in milliseconds
 */
extern uint32_t millis(void);

#endif

/** @} */
