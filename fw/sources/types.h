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
 * @file    types.h
 * @brief   Standard system types
 *
 * @{
 */

#ifndef __TYPES_H
#define __TYPES_H

#include <inttypes.h>
#include <stdlib.h> /* NULL, size_t,... */

#ifndef bool
    #define bool _Bool
    #define true 1
    #define false 0
#endif

/**
 * macros for converting two strings into single name, e.g. for HAL
 * variables, etc.
 *
 * Due to the way the gcc preprocessor works, the macro must be resolved
 * twice, hence the construction below
 */
#define JOIN_(str1, str2) str1 ## str2
#define JOIN(str1, str2) JOIN_(str1, str2)

#endif

/** @} */
