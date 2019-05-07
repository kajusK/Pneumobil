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
 * @file    assert.h
 * @brief   ASSERT macros
 *
 * @addtogroup utils
 * @{
 */

#ifndef __UTILS_ASSERT_H
#define __UTILS_ASSERT_H

#include "drivers/sys.h"

#ifndef ASSERT
    #define ASSERT(condition) \
        if (!(condition)) { \
            Sysd_Reboot(); \
            while (1); \
        }
#endif

#ifndef ASSERT_NOT
    #define ASSERT_NOT(condition) ASSERT(!(condition))
#endif

#endif

/** @} */
